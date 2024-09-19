import re
import networkx as nx
from graphviz import Digraph

def parse_environment(file_path):
    """
    Parses the environment file to determine which nodes are tainted.
    Returns a dictionary mapping node names to their tainted status.
    """
    tainted_nodes = {}
    try:
        with open(file_path, 'r') as file:
            content = file.readlines()
    except FileNotFoundError:
        print(f"Error: The file '{file_path}' was not found.")
        return tainted_nodes

    print("Parsing environment file...\n")
    # Process each line in the environment file
    for line in content:
        line = line.strip()
        if not line or line.startswith('ENVIRONMENT:'):
            continue

        # Example line format:
        # arg5_1 = [Checked=TPTR[ -1.000000e+00 ], PtrType=TPTR[ 1.000000e+00 ]]
        match = re.match(r'^(.+?)\s*=\s*\[Checked=(\w+)\[.*\], PtrType=(\w+)\[.*\]\]', line)
        if match:
            node_name = match.group(1).strip()
            checked_type = match.group(2).strip()
            ptr_type = match.group(3).strip()

            # Determine tainted status based on types
            tainted_types = {'TPTR', 'TNTARR', 'TARR'}
            if checked_type in tainted_types or ptr_type in tainted_types:
                tainted_nodes[node_name] = True
            else:
                tainted_nodes[node_name] = False
        else:
            print(f"Warning: Unable to parse environment line: '{line}'")
    print("Environment parsing completed.\n")
    return tainted_nodes

def parse_constraints_iterative(file_path, tainted_nodes):
    """
    Parses the constraints from the given file iteratively and returns a list of edges and structure nodes.
    Ensures that all taint flows, including those through structures, are captured.
    Considers the tainted status of nodes when adding edges between structures and their members.
    """
    all_constraints = []
    edges = []
    discovered = set(['TPTR'])  # Starting with TPTR
    structure_nodes = set()

    try:
        with open(file_path, 'r') as file:
            content = file.read()
    except FileNotFoundError:
        print(f"Error: The file '{file_path}' was not found.")
        return edges, structure_nodes

    # Remove 'Choke Analysis:' and split by newlines only
    content = content.replace('Choke Analysis:', '')
    constraints = [c.strip() for c in content.split('\n') if c.strip()]
    all_constraints = constraints

    print(f"Starting iterative traversal with root node: TPTR\n")
    print(f"Total constraints to process: {len(all_constraints)}\n")

    added_new = True
    iteration = 0
    max_iterations = 1000  # Prevent infinite loops

    while added_new and iteration < max_iterations:
        added_new = False
        iteration += 1
        print(f"--- Iteration {iteration} ---")
        for idx, constraint in enumerate(all_constraints, 1):
            if '>=' not in constraint:
                continue
            # Updated regex to capture 'from_node', 'to_node', and 'Reason'
            match = re.match(r'^(.+?)\s*\([PC]\)\s*>=\s*([^,]+),\s*Reason:\s*(.+)$', constraint)
            if match:
                from_node = match.group(1).strip()
                to_node = match.group(2).strip()
                reason = match.group(3).strip()

                print(f"Processing Constraint {idx}: '{constraint}'")
                print(f"  From Node: '{from_node}'")
                print(f"  To Node: '{to_node}'")
                print(f"  Reason: '{reason}'")

                # Identify if from_node or to_node is a structure
                if from_node.startswith('struct '):
                    structure_nodes.add(from_node)
                if to_node.startswith('struct '):
                    structure_nodes.add(to_node)

                # Determine edge addition based on Reason
                if 'Pointer within a Tainted Structure' in reason:
                    # Extract the structure name
                    struct_match = re.search(r'Pointer within a Tainted Structure\s*"(.+?)"', reason)
                    if struct_match:
                        struct_name = f'struct {struct_match.group(1)}'
                        print(f"  Extracted Structure Name: '{struct_name}'")
                        # Only add edge if the member is tainted
                        if tainted_nodes.get(to_node, False):
                            # Add edge from from_node to struct_name
                            if struct_name not in discovered:
                                edges.append((from_node, struct_name))
                                discovered.add(struct_name)
                                print(f"  Added edge: '{from_node}' -> '{struct_name}'")
                                added_new = True
                            # Add edge from struct_name to to_node
                            if (struct_name, to_node) not in edges:
                                edges.append((struct_name, to_node))
                                discovered.add(to_node)
                                print(f"  Added edge: '{struct_name}' -> '{to_node}'")
                                added_new = True
                    else:
                        # If structure name not found, add direct edge if to_node is tainted
                        if from_node in discovered and to_node not in discovered and tainted_nodes.get(to_node, False):
                            edges.append((from_node, to_node))
                            discovered.add(to_node)
                            print(f"  Added edge: '{from_node}' -> '{to_node}'")
                            added_new = True

                elif 'Tainted Pointer to the Structure' in reason:
                    # Extract the structure name and the special _TPtr node
                    struct_match = re.search(r'Tainted Pointer to the Structure\s*"(.+?)"', reason)
                    if struct_match:
                        struct_name = f'struct {struct_match.group(1)}'
                        special_tptr = from_node
                        print(f"  Extracted Structure Name: '{struct_name}'")
                        print(f"  Special TPtr Node: '{special_tptr}'")

                        # Remove the edge between TPTR and struct if it exists
                        if ('TPTR', struct_name) in edges:
                            edges.remove(('TPTR', struct_name))
                            discovered.discard(struct_name)
                            print(f"  Removed edge: 'TPTR' -> '{struct_name}'")

                        # Connect TPTR to the special _TPtr node
                        if ('TPTR', special_tptr) not in edges:
                            edges.append(('TPTR', special_tptr))
                            discovered.add(special_tptr)
                            print(f"  Added edge: 'TPTR' -> '{special_tptr}'")
                            added_new = True

                        # Connect the special _TPtr node to the structure
                        if (special_tptr, struct_name) not in edges:
                            edges.append((special_tptr, struct_name))
                            discovered.add(struct_name)
                            print(f"  Added edge: '{special_tptr}' -> '{struct_name}'")
                            added_new = True

                        continue  # Skip the default edge addition below

                elif 'Structure within a pointer' in reason:
                    # Extract the structure name
                    struct_match = re.search(r'Structure within a pointer\s*"(.+?)"', reason)
                    if struct_match:
                        struct_name = f'struct {struct_match.group(1)}'
                        print(f"  Extracted Structure Name: '{struct_name}'")
                        # Only add edge if to_node is tainted
                        if tainted_nodes.get(to_node, False):
                            # Add edge from struct_name to to_node
                            if (struct_name, to_node) not in edges:
                                edges.append((struct_name, to_node))
                                discovered.add(to_node)
                                print(f"  Added edge: '{struct_name}' -> '{to_node}'")
                                added_new = True
                else:
                    # Default behavior: add direct edge if to_node is tainted
                    if from_node in discovered and to_node not in discovered and tainted_nodes.get(to_node, False):
                        edges.append((from_node, to_node))
                        discovered.add(to_node)
                        print(f"  Added edge: '{from_node}' -> '{to_node}'")
                        added_new = True
            else:
                print(f"  Warning: Unable to parse constraint: '{constraint}'")
        print(f"--- End of Iteration {iteration} ---\n")

    if iteration >= max_iterations:
        print(f"Warning: Reached maximum iterations ({max_iterations}). Possible infinite loop.\n")

    return edges, structure_nodes

def build_graph(edges, structure_nodes):
    """
    Builds a directed graph from the list of edges.
    Assigns a 'structure' attribute to structure nodes for visualization.
    """
    G = nx.DiGraph()
    G.add_edges_from(edges)

    # Assign 'structure' attribute
    for node in G.nodes():
        if node in structure_nodes:
            G.nodes[node]['structure'] = True
        else:
            G.nodes[node]['structure'] = False

    return G

def identify_leaf_nodes(G, structure_nodes):
    """
    Identifies leaf nodes in the graph (nodes with no outgoing edges).
    Excludes structure nodes from being considered as leaf nodes.
    """
    leaf_nodes = [node for node in G.nodes() if G.out_degree(node) == 0 and node not in structure_nodes]
    return leaf_nodes

def assign_scores(G, leaf_nodes):
    """
    Assigns scores to each node in the graph based on the rules:
    - Leaf nodes get a score of 1.
    - Other nodes get the sum of scores of their children.
    """
    scores = {node: 0 for node in G.nodes()}

    # Initialize leaf nodes with score 1
    for leaf in leaf_nodes:
        scores[leaf] = 1

    # Reverse the graph to process from leaves upwards
    reverse_G = G.reverse(copy=True)

    # Perform a topological sort.
    try:
        topo_order = list(nx.topological_sort(reverse_G))
    except nx.NetworkXUnfeasible:
        print("Graph contains cycles. Scores may not be accurately assigned.")
        topo_order = list(G.nodes())

    for node in topo_order:
        if node in leaf_nodes:
            continue
        # Sum the scores of all children
        children = list(G.successors(node))
        node_score = sum([scores[child] for child in children])
        scores[node] = node_score

    return scores

def visualize_graph(G, scores, tainted_nodes, output_path='graph'):
    """
    Visualizes the graph using Graphviz, displaying scores on each node.
    Structure nodes are represented with a box shape, others with the default ellipse.
    Tainted nodes are colored differently for clarity.
    """
    dot = Digraph(comment='Constraint Graph', format='png')

    for node in G.nodes():
        label = f"{node}\nScore: {scores.get(node, 0)}"
        if G.nodes[node].get('structure', False):
            dot.node(node, label, shape='box', style='filled', color='lightblue')
        else:
            if tainted_nodes.get(node, False):
                dot.node(node, label, style='filled', color='red')
            else:
                dot.node(node, label)

    for from_node, to_node in G.edges():
        dot.edge(from_node, to_node)

    dot.render(output_path, view=False)
    print(f"Graph has been saved to {output_path}.png\n")

def main():
    # Paths to the constraints and environment files
    constraints_file = 'constraints_temp.txt'
    env_file = 'constraint_env.txt'

    # Parse the environment to get tainted nodes
    tainted_nodes = parse_environment(env_file)
    print("\nTainted Nodes:")
    for node, tainted in tainted_nodes.items():
        status = 'Tainted' if tainted else 'Not Tainted'
        print(f"{node}: {status}")
    print()

    # Parse constraints iteratively to get edges and identify structure nodes
    edges, structure_nodes = parse_constraints_iterative(constraints_file, tainted_nodes)

    print("\nParsed Edges:")
    for edge in edges:
        print(edge)
    print()

    print("Identified Structure Nodes:")
    for struct in structure_nodes:
        print(struct)
    print()

    # Build the graph
    G = build_graph(edges, structure_nodes)

    # Identify leaf nodes, excluding structures
    leaf_nodes = identify_leaf_nodes(G, structure_nodes)
    print("\nLeaf Nodes:", leaf_nodes, "\n")

    # Assign scores
    scores = assign_scores(G, leaf_nodes)
    print("\nNode Scores:")
    for node, score in scores.items():
        print(f"{node}: {score}")
    print()

    # Visualize the graph
    visualize_graph(G, scores, tainted_nodes)

if __name__ == "__main__":
    main()

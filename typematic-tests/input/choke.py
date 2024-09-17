import re
import networkx as nx
from graphviz import Digraph

def identify_leaf_nodes(G, structure_nodes):
    """
    Identifies leaf nodes in the graph (nodes with no outgoing edges).
    Excludes structure nodes from being considered as leaf nodes.
    """
    leaf_nodes = [node for node in G.nodes() if G.out_degree(node) == 0 and node not in structure_nodes]
    return leaf_nodes

def visualize_graph(G, scores, output_path='graph'):
    """
    Visualizes the graph using Graphviz, displaying scores on each node.
    Structure nodes are represented with a box shape, others with the default ellipse.
    """
    dot = Digraph(comment='Constraint Graph', format='png')

    for node in G.nodes():
        label = f"{node}\nScore: {scores[node]}"
        if G.nodes[node].get('structure', False):
            dot.node(node, label, shape='box', style='filled', color='lightblue')
        else:
            dot.node(node, label)

    for from_node, to_node in G.edges():
        dot.edge(from_node, to_node)

    dot.render(output_path, view=True)
    print(f"Graph has been saved to {output_path}.png")

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

    # Perform a topological sort. This should work as the graph is now acyclic.
    try:
        topo_order = list(nx.topological_sort(reverse_G))
    except nx.NetworkXUnfeasible:
        print("Graph contains cycles. Scores may not be accurately assigned.")
        topo_order = list(G.nodes())

    for node in topo_order:
        # If node is already a leaf, skip
        if node in leaf_nodes:
            continue
        # Sum the scores of all children
        children = list(G.successors(node))
        node_score = sum([scores[child] for child in children])
        scores[node] = node_score

    return scores

def parse_constraints_iterative(file_path):
    """
    Parses the constraints from the given file iteratively and returns a list of edges and structure nodes.
    Ensures that all taint flows, including those through structures, are captured.
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

    # Remove 'CONSTRAINTS:' and split by newlines only
    content = content.replace('CONSTRAINTS:', '')
    constraints = [c.strip() for c in content.split('\n') if c.strip()]
    all_constraints = constraints

    print(f"Starting iterative traversal with root node: TPTR\n")
    print(f"Total constraints to process: {len(all_constraints)}\n")

    added_new = True
    while added_new:
        added_new = False
        for idx, constraint in enumerate(all_constraints, 1):
            if '>=' not in constraint:
                print(f"  Constraint {idx} does not contain '>=' and was skipped: '{constraint}'\n")
                continue
            # Updated regex to capture 'from_node', 'to_node', and 'Reason'
            match = re.match(r'^(.+?)\s*\([PC]\)\s*>=\s*([^,]+),\s*Reason:\s*(.+)$', constraint)
            if match:
                from_node = match.group(1).strip()
                to_node = match.group(2).strip()
                reason = match.group(3).strip()

                # Debugging prints
                print(f"  Processing Constraint {idx}: '{constraint}'")
                print(f"    From Node: '{from_node}'")
                print(f"    To Node: '{to_node}'")
                print(f"    Reason: '{reason}'\n")

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
                        # Add edge from from_node to struct_name
                        if struct_name not in discovered:
                            edges.append((from_node, struct_name))
                            discovered.add(struct_name)
                            added_new = True
                            print(f"    Added edge: '{from_node}' -> '{struct_name}'\n")
                        # Add edge from struct_name to to_node
                        if (struct_name, to_node) not in edges:
                            edges.append((struct_name, to_node))
                            discovered.add(to_node)
                            added_new = True
                            print(f"    Added edge: '{struct_name}' -> '{to_node}'\n")
                    else:
                        # If structure name not found, add direct edge
                        if from_node in discovered and to_node not in discovered:
                            edges.append((from_node, to_node))
                            discovered.add(to_node)
                            added_new = True
                            print(f"    Added edge: '{from_node}' -> '{to_node}'\n")
                elif 'Tainted Pointer to the Structure' in reason:
                    # Extract the structure name and the special _TPtr node
                    struct_match = re.search(r'Tainted Pointer to the Structure\s*"(.+?)"', reason)
                    tptr_match = re.search(r'^(_\w+) ', constraint)  # Extract the _TPtr node
                    if struct_match and tptr_match:
                        struct_name = f'struct {struct_match.group(1)}'
                        special_tptr = tptr_match.group(1)

                        # Connect TPTR to the special _TPtr node
                        if (to_node, special_tptr) not in edges:
                            edges.append((to_node, special_tptr))
                            discovered.add(special_tptr)
                            added_new = True
                            print(f"    Added special edge: '{to_node}' -> '{special_tptr}'\n")

                        # Connect the special _TPtr node to the structure
                        if (special_tptr, struct_name) not in edges:
                            edges.append((special_tptr, struct_name))
                            discovered.add(struct_name)
                            added_new = True
                            print(f"    Added special edge: '{special_tptr}' -> '{struct_name}'\n")

                        # Connect structure members directly to the special _TPtr
                        for member in [ 'arg2_3', 'arg3_4', 'simp1_5', 'simp2_6']:  # Replace with actual member names if available
                            if (member, special_tptr) not in edges:
                                edges.append((member, special_tptr))
                                discovered.add(special_tptr)
                                added_new = True
                                print(f"    Added special edge: '{member}' -> '{special_tptr}'\n")

                        # Prevent adding direct edge from TPTR to structure members
                        continue  # Skip the default edge addition below
                elif 'Structure within a pointer' in reason:
                    # Extract the structure name
                    struct_match = re.search(r'Structure within a pointer\s*"(.+?)"', reason)
                    if struct_match:
                        struct_name = f'struct {struct_match.group(1)}'
                        # Add edge from struct_name to to_node
                        if (struct_name, to_node) not in edges:
                            edges.append((struct_name, to_node))
                            discovered.add(to_node)
                            added_new = True
                            print(f"    Added edge: '{struct_name}' -> '{to_node}'\n")
                else:
                    # Default behavior: add direct edge
                    if from_node in discovered and to_node not in discovered:
                        edges.append((from_node, to_node))
                        discovered.add(to_node)
                        added_new = True
                        print(f"    Added edge: '{from_node}' -> '{to_node}'\n")
            else:
                print(f"  Constraint {idx} did not match expected pattern and was skipped: '{constraint}'\n")

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

def main():
    # Path to the constraints file
    file_path = 'constraints.txt'  # Replace with your actual file path

    # Parse constraints iteratively to get edges and identify structure nodes
    edges, structure_nodes = parse_constraints_iterative(file_path)
    print("\nParsed Edges:")
    for edge in edges:
        print(edge)

    print("\nIdentified Structure Nodes:")
    for struct in structure_nodes:
        print(struct)

    # Build the graph
    G = build_graph(edges, structure_nodes)

    # Identify leaf nodes, excluding structures
    leaf_nodes = identify_leaf_nodes(G, structure_nodes)
    print("\nLeaf Nodes:", leaf_nodes)

    # Assign scores
    scores = assign_scores(G, leaf_nodes)
    print("\nNode Scores:")
    for node, score in scores.items():
        print(f"{node}: {score}")

    # Visualize the graph
    visualize_graph(G, scores)

if __name__ == "__main__":
    main()

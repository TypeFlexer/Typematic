let target_file = ref ""

let set_target_file f = target_file := f
let set_stdchecked () = Lexer.stdchecked := true
let set_stdcheckcbox () = Lexer.stdcheckcbox := true

let read_whole_file filename =
    let ch = open_in filename in
    let s = really_input_string ch (in_channel_length ch) in
    close_in ch;
    s

let format s result =
    let len = String.length s in
    let rec iter result i out = match result with
    | (start_pos, end_pos, replacement)::body -> 
        iter body end_pos (String.concat "" [out; (String.sub s i (start_pos - i)); replacement])
    | [] -> (String.concat "" [out; (String.sub s i (len-i))])
    in
    print_string (iter result 0 "")

let () =
  let speclist =
    [("-f", Arg.String (set_target_file), "Target tainted-c code to convert to c");
     ("-c", Arg.Unit (set_stdchecked), "Assume <stdtainted.h> is included, to use Tainted C shorthands");
    ] in
    let usage_msg = "C3 tool" in
    Arg.parse speclist print_endline usage_msg;
    let s = read_whole_file !target_file in
	let lexbuf = Lexing.from_string s in
    try 
        let result = Parser.main Lexer.keyword lexbuf in
        format s result
    with
    | _ ->
        let pos =  Lexing.lexeme_start_p lexbuf in
        Printf.fprintf stderr "At %d:%d syntax error (token %s).\n%!" pos.pos_lnum (pos.pos_cnum-pos.pos_bol) (Lexing.lexeme lexbuf); 
        exit 1


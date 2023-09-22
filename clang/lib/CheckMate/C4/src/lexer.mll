{
  open Parser
  open Hack
  exception Error of string

(* NOTE: More keywords here https://github.com/correctcomputation/checkedc/blob/master/include/stdchecked.h *)

  let stdchecked = ref false;;
  let stdcheckcbox = ref false;;
  (*
#define ptr _Ptr
#define array_ptr _Array_ptr
#define nt_array_ptr _Nt_array_ptr
#define tptr _TPtr
#define tstruct Tstruct
#define tarray_ptr _TArray_ptr
#define tnt_array_ptr _TNt_array_ptr
#define checked _Checked
#define tainted _Tainted
#define mirror _Mirror
#define nt_checked _Nt_checked
#define unchecked _Unchecked
#define bounds_only _Bounds_only
#define where _Where
#define dynamic_check _Dynamic_check
#define dynamic_bounds_cast _Dynamic_bounds_cast
#define assume_bounds_cast _Assume_bounds_cast
#define tainted_dynamic_bounds_cast _Tainted_Dynamic_bounds_cast
#define tainted_assume_bounds_cast _Tainted_Assume_bounds_cast
#define return_value _Return_value
 *)

  let brace_depth = ref None
    let create_hashtable size init =
        let tbl = Hashtbl.create size in
        List.iter (fun (key, data) -> Hashtbl.add tbl key data) init;
        tbl

    let keyword_table =
        create_hashtable 100 [
                ("t_atof", TATOF);
                ("t_atoi" ,TATOI);
                ( "t_atol" , TATOL);
                ( "t_atoll" , TATOLL);
                ( "t_strtod" , TSTRTOD);
                ( "t_strtof" , TSTRTOF);
                ( "t_strtold" , TSTRTOLD);
                ( "t_strtol" , TSTRTOL);
                ( "t_strtoll" , TSTRTOLL);
                ( "t_strtoul" , TSTRTOUL);
                ( "t_strtoull" , TSTRTOULL);
                ( "t_aligned_alloc" , TALIGNEDALLOC);
                ("__t_sprintf_chkcbx", TSPRINTFCHKCBX);
                ( "t_free" , TFREE);
                ( "t_getenv" , TGETENV);
                ( "t_atexit" , TATEXIT);
                ( "t_atquick_exit" , TATQUICKEXIT);
                ( "t_system" , TSYSTEM);
                ( "t_bsearch" , TBSEARCH);
                ( "t_qsort" , TQSORT);
                ( "t_mblen" , TMBLEN);
                ( "t_mbtowc" , TMBTOWC);
                ( "t_mbstowcs" , TMBSTOWCS);
                ( "t_wcstombs" , TWCSTOMBS);
                ( "t_malloc" , TMALLOC);
                ( "t_realloc" , TREALLOC);
                ( "__t_builtin_object_size" , TBUILTINOBJECTSIZE);
                ( "__t_builtin___sprintf_chk" , TBUILTINSPRINTFCHK);
                ( "__t_snprintf_chk" , TSNPRINTFCHK);
                ( "__t_builtin___snprintf_chk" , TBUILTINSNPRINTFCHK);
                ( "__t_vsprintf_chk" , TVSPRINTFCHK);
                ( "__t_builtin___vsprintf_chk" , TBUILTINVSPRINTFCHK);
                ( "__t_vsnprintf_chk" , TVSNNPRINTFCHK);
                ( "__t_builtin___vsnprintf_chk" , TBUILTINVSNPRINTFCHK);
                ( "__t_fprintf_chk" , TFRINTFCHK);
                ( "__t_builtin___fprintf_chk" , TBUILTINFPRINTFCHK);
                ( "__t_printf_chk" , TPRINTFCHK);
                ( "t_printf" , TPRINTF);
                ( "__t_builtin___printf_chk" , TBUILTINPRINTFCHK);
                ( "__t_vfprintf_chk" , TVFPRINTFCHK);
                ( "__t_builtin___vfprintf_chk" , TBUILTINVFPRINTFCHK);
                ( "__t_builtin___vprintf_chk" , TBULTINVPRINTFCHK);
                ("t_frexp", TFREEXP);
                ("t_frexpf", TFREEXPF);
                ("t_frexpl", TFREEXPL);
                ("t_modf", TMODF);
                ("t_modff", TMODFF);
                ("t_modfl", TMODFL);
                ("t_remquo", TREMQUO);
                ("t_remquof", TREMQUOF);
                ("t_remquol", TREMQUOL);
                ("t_nan", TNAN);
                ("t_fclose", TFCLOSE);
                ("t_fread", TFREAD);
                ("t_fopen", TFOPEN);
                ("t_fseek", TFSEEK);
                ("t_ftell", TFTELL);
                ("t_ferror", TFERROR);
                ("t_rewind",TREWIND);
                ("t_nanf", TNANF);
                ("t_nanl", TNANL);
                ("t_isnan", TISNAN);
                ("t_isinf", TISINF);
                ("_t_errno", TERRNO);
                ("t_memcpy", TMEMCPY);
                ("t_memmove", TMEMMOVE);
                ("t_memset", TMEMSET);
                ("t_strcpy", TSTRCPY);
                ("t_strncpy", TSTRNCPY);
                ("t_strcat", TSTRCAT);
("t_strncat", TSTRNCAT);
                ("t_memcmp", TMEMCMP);
                ("t_strcmp", TSTRCMP);
                ("t_strcoll", TSTRCOLL);
                ("t_strncmp", TSTRNCMP);
                ("t_strxfrm", TSTRXFRM);
("t_memchr", TMEMCHR);
                ("t_strchr", TSTRCHR);
                ("t_strcspn", TSTRCSPN);
                ("t_strpbrk", TSTRPBRK);
                ("t_strrchr", TSTRRCHR);
                ("t_strspn", TSTRSPN);
                ("t_strstr", TSTRSTR);
                ("t_strtok", TSTRTOK);
                ("t_strerror", TSTRERROR);
                ("t_strlen", TSTRLEN);
                ("t_strdup", TSTRDUP);
        ]

}

let id = ['a'-'z' 'A'-'Z' '_'] ['a'-'z' 'A'-'Z' '0'-'9' '_']*
let bounds = "bounds" | "count" | "byte_count"
let pid = "malloc" | "calloc" | "free" | "realloc" | "memcpy"

rule keyword = parse
| ['/']['*']  { let b = Buffer.create 17 in Buffer.add_string b "/*"; comment b lexbuf }
| ['/']['/']   { let b = Buffer.create 17 in Buffer.add_string b "//"; line_comment b lexbuf }
| [' ' '\t']     { keyword lexbuf }
| ['#']"pragma" { let start_p = Lexing.lexeme_start lexbuf in
                        let _ = pragma lexbuf in
                        let end_p = Lexing.lexeme_end lexbuf in
                        Lexing.new_line lexbuf;
                        PRAGMA(start_p,end_p)  }
| ['#']"include"[' ' '\t']*"<stdchecked.h>" { stdchecked := true; CHECKED }
| ['#']"include"[' ' '\t']*"<stdtainted.h>" { stdcheckcbox := false; CHECKED }
| '"' { let b = Buffer.create 17 in Buffer.add_char b '"'; read_string b lexbuf }
| '\'' { let b = Buffer.create 17 in Buffer.add_char b '\''; read_char b lexbuf }
| "_Itype_for_any" | "_For_any" { brace_depth := Some(0); FORANY }
| "_Ptr" | "_Array_ptr" | "_Nt_array_ptr" { PTR }
| "_TPtr" | "_TArray_ptr" | "_TNt_array_ptr" { TPTR }
| "Tstruct" { TSTRUCT}
| "_Checked" | "_Unchecked" | "_Nt_checked" { CHECKED }
| "_Tainted" {TAINTED}
| "_Callback" {CALLBACK}
| "_Mirror" {MIRROR}
| "_Dynamic_check" { DYNCHECK }
| "t_malloc" {TMALLOC}
| "t_free" {TFREE}
| "t_realloc" {TREALLOC}
| "t_fclose" {TFCLOSE}
| "t_fopen" {TFOPEN}
| "t_fread" {TFREAD}
| "t_fseek" {TFSEEK}
| "t_ftell" {TFTELL}
| "_t_errno" {TERRNO}
| "t_memcpy" {TMEMCPY}
| "t_strncmp" {TSTRNCMP}
| "t_strlen" {TSTRLEN}
| "t_strncmp" {TSTRNCMP}
| "t_sprintf" {TSPRINTF}
| "t_strchr" {TSTRCHR}
| "_Assume_bounds_cast" | "_Dynamic_bounds_cast" | "_Tainted_Assume_bounds_cast" | "_Tainted_Dynamic_bounds_cast" {
ASSUME_CAST }
(* Shorthands -- could limit only if !stdchecked, but won't work if not directly included *)
| "ptr" | "array_ptr" | "nt_array_ptr" { if !stdchecked then PTR else ID(Lexing.lexeme lexbuf) }
| "tptr" | "tarray_ptr" | "tnt_array_ptr" { if !stdcheckcbox then TPTR else ID(Lexing.lexeme lexbuf) }
| "Tstruct" { if !stdcheckcbox then TSTRUCT else ID(Lexing.lexeme lexbuf) }
| "checked" | "unchecked" | "nt_checked" {if !stdcheckcbox then CHECKED else ID(Lexing.lexeme lexbuf) }
| "dynamic_check" { if !stdchecked then DYNCHECK else ID(Lexing.lexeme lexbuf) }
| "assume_bounds_cast" | "dynamic_bounds_cast" | "tainted_assume_bounds_cast" | "tainted_dynamic_bounds_cast"
{ if !stdchecked then ASSUME_CAST else ID(Lexing.lexeme lexbuf) }
| pid { PID(Lexing.lexeme lexbuf) }
| id as word
{
    try
        let token = Hashtbl.find keyword_table word in
        token
    with Not_found ->
        ID word
}
| "," { COMMA }
| ";" { (match !brace_depth with
          Some 0 -> brace_depth := None; clear_tyvars() (* forall applied to a prototype; stop looking *)
        | _ -> ()); ANY(Lexing.lexeme lexbuf) }
| "<" { LANGLE }
| ">" { RANGLE }
| "(" { LPAREN }
| ")" { RPAREN }
| "{" { (match !brace_depth with
          Some n -> brace_depth := Some(n+1)
        | None -> ()); ANY(Lexing.lexeme lexbuf) }
| "}" { (match !brace_depth with
          Some n -> if n = 1 then (brace_depth := None; clear_tyvars()) else brace_depth := Some(n-1)
        | None -> ()); ANY(Lexing.lexeme lexbuf) }
| ":" { let start_p = Lexing.lexeme_start lexbuf in
        let c = colon lexbuf in
        let end_p = Lexing.lexeme_end lexbuf in
        if c = 1 then COLONITYPE(start_p,end_p)
        else if c = 2 then COLONBOUNDS(start_p,end_p)
        else COLON }
| "\n" { Lexing.new_line lexbuf; keyword lexbuf; }
| eof { EOF }
| _ as c { ANY(String.make 1 c) }

and colon =
  parse
  | "itype" {1 } 
  | bounds { 2 }
  | "\n" { Lexing.new_line lexbuf; colon lexbuf; }
  | [' ' '\t'] { colon lexbuf }
  | _ { 3 } (* FIX: eats up a character *)

and pragma =
  parse
 | "\n" { COLON }
 | _ { pragma lexbuf }

and comment buf =
  parse
| ['*']['/']       { Buffer.add_string buf "*/"; ANY (Buffer.contents buf) }
 | "\n" { Lexing.new_line lexbuf; Buffer.add_char buf '\n'; comment buf lexbuf; }
  | _ as c { Buffer.add_char buf c; comment buf lexbuf }

and line_comment buf =
  parse
 | [^ '\n'] as c { Buffer.add_char buf c; line_comment buf lexbuf }
 | "\n" { Lexing.new_line lexbuf; Buffer.add_char buf '\n';  ANY (Buffer.contents buf) }

and read_string buf =
  parse
  | '"'       { Buffer.add_char buf '"'; ANY (Buffer.contents buf) }
  | '\\' '\n' { Lexing.new_line lexbuf;  Buffer.add_char buf '\\'; Buffer.add_char buf '\n'; read_string buf lexbuf; }
  | '\\' '\\'  { Buffer.add_char buf '\\'; Buffer.add_char buf '\\'; read_string buf lexbuf }
  | '\\' '"'  { Buffer.add_char buf '\\'; Buffer.add_char buf '"'; read_string buf lexbuf }
  | _ as c { Buffer.add_char buf c; read_string buf lexbuf }

and read_char buf =
  parse
  | '\''       { Buffer.add_char buf '\''; ANY (Buffer.contents buf) }
  | '\\' '\''  { Buffer.add_char buf '\\'; Buffer.add_char buf '\''; read_char buf lexbuf }
  | '\\' '\\'  { Buffer.add_char buf '\\'; Buffer.add_char buf '\\'; read_char buf lexbuf }
  | _ as c { Buffer.add_char buf c; read_char buf lexbuf }

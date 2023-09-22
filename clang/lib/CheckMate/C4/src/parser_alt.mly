
%token PTR
%token TSTRUCT
%token ITYPE
%token <string> ANY
%token <int * int> PRAGMA
%token <string> ID
%token EOF
%token LANGLE
%token RANGLE
%token LPAREN
%token RPAREN
%token FORANY
%token COLON
%token CHECKED
%token DYNCHECK

%start <(int*int*string) list> main

%%

main:
| p = pointer m = main { ($startpos.pos_cnum , $endpos(p).pos_cnum, p)::m }
| p = annot m = main { match p with Some c -> c::m | None -> m }
| LPAREN m = main { m }
| RPAREN m = main { m }
| LANGLE m = main { m }
| RANGLE m = main { m }
| COLON m = main { m }
| ID m = main { m }
| ANY m = main { m }
| EOF { [] }

annot:
/* add INCLUDE here; drop stdchecked.h (and note it in lexer) */
| CHECKED { Some ($startpos.pos_cnum, $endpos.pos_cnum, "") }
| TSTRUCT {Some ($startpos.pos_cnum, $endpos.pos_cnum, "struct")}
| p = PRAGMA { let (s,e) = p in Some (s, e, "") }
| DYNCHECK LPAREN insidebounds* RPAREN { Some ($startpos.pos_cnum, $endpos.pos_cnum, "") }
| FORANY LPAREN ID RPAREN { Some ($startpos.pos_cnum, $endpos.pos_cnum, "") }
| COLON b = bounds
    { if b then Some ($startpos.pos_cnum, $endpos.pos_cnum, "") else None }
| COLON itype bounds*
    { Some ($startpos.pos_cnum, $endpos.pos_cnum, "") }
| TSPRINTFCHKCBX {($startpos.pos_cnum, $endpos.pos_cnum, "__sprintf_chkcbx")}

bounds:
| k = ID LPAREN insidebounds* RPAREN { k = "count" || k = "bounds" || k = "byte_count"  }

insidebounds:
| LPAREN insidebounds* RPAREN { None }
| pointer { None }
| LANGLE { None }
| RANGLE { None }
| COLON { None }
| ID { None }
| ANY { None }

itype:
| ITYPE LPAREN insideitype* RPAREN { None }

insideitype:
| pointer { None }
| ID { None }
| ANY { None }

tstruct:
| TSTRUCT { String.concat  " "}

pointer:
| PTR LANGLE p = pointer RANGLE { String.concat "" [p; " *"] }
| PTR LANGLE s = insideptr RANGLE { String.concat "" [s; " *"]}
(* TODO: correct replacement *)
| PTR LANGLE fp = fpointer RANGLE name = ID 
  { let (ret,params) = fp in String.concat "" [ret; "(*"; name; ")"; params] }

fpointer:
| ret = rettype LPAREN lst = option(paramlist) RPAREN 
  { (ret, String.concat "" (Option.to_list lst)) }

rettype: 
| c = TSTRUCT tstruct {c}
| c = ID { c } 
| c = pointer { c }

paramlist:
| lst = separated_list(ANY, param)
  { String.concat "" ["("; String.concat "," lst; ")"] }

param:
| c = TSTRUCT tstruct {c}
| c = ID { c }
| c = pointer { c }

insideptr:
| c = TSTRUCT tstruct {c} 
| c = ANY s = insideptr { String.concat "" [c; s]}
/* This is not properly capturing whitespace: it assumes there's a space between tokens, but that's not necessarily so. Need to fix lexer.  */
| c = ID s = insideptr { String.concat " " [c; s]}
| c = ANY { c }
| c = ID { c }


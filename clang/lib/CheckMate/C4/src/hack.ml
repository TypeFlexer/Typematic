let fl () = Stdlib.flush stdout
          
let tyvars : string list ref = ref []
let note_tyvar t = (* Printf.printf "noting %s\n" t; fl(); *) tyvars := t::!tyvars
let is_tyvar t = (* Printf.printf "looking for %s\n" t; fl(); *) List.mem t !tyvars
let clear_tyvars () = (* Printf.printf "clearing all vars\n"; fl(); *) tyvars := [] 
let tostring o = 
  match o with 
  | Some s -> s 
  | None -> ""

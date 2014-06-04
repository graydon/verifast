(**
 * Shape analyser frontend
 *
 * The frontend is the glue before the backend: it makes sure the file is
 * parsed to an AST, the relevant part of the AST is taken, and passes
 * this AST to the backend. The backend returns a list of changes (e.g.
 * insert an annotation at some position) to the frontend. The frontend
 * modifies the source file by applying these changes.
 *
 * Files are read but changes are not written to disk (the new contents is
 * passed as string). This makes it easier for the IDE to support undo.
 *)
 

open Shape_analysis_backend
open Parser (* for file_type *)
open Changelog

(**
 * The main function of the shape analyser frontend (see specs at top of
 * this file).
 * The contents is passed as a filename (instead of e.g. the real contents)
 * because the rest of VeriFast works like that.
 *)
let shape_analyse_frontend path include_paths position : string =
  let ast_all = parse_c_file path (fun _ _ -> ()) (fun _ -> ()) include_paths in
  let ast_function =
    ast_all (* TODO: extract function that covers position from AST *)
  in
  let changes = shape_analyse_backend ast_function in
  let file_contents = "" (* TODO: read file to string *) in
  changelog_apply file_contents changes
;;

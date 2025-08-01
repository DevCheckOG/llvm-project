//==--- DiagnosticSerializationKinds.td - serialization diagnostics -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

let Component = "Serialization" in {
let CategoryName = "AST Deserialization Issue" in {

def err_fe_unable_to_read_pch_file : Error<
    "unable to read PCH file %0: '%1'">;
def err_fe_not_a_pch_file : Error<
    "input is not a PCH file: '%0'">;
def err_fe_ast_file_malformed : Error<
    "malformed or corrupted precompiled file: '%0'">, DefaultFatal;
def err_fe_pch_malformed_block : Error<
    "malformed block record in PCH file: '%0'">, DefaultFatal;
def err_fe_ast_file_modified : Error<
    "file '%0' has been modified since the "
    "%select{precompiled header|module file|precompiled file}1 '%2' was built"
    ": %select{size|mtime|content}3 changed%select{| (was %5, now %6)}4">,
    DefaultFatal;
def err_fe_pch_file_overridden : Error<
    "file '%0' from the precompiled header has been overridden">;
def note_ast_file_required_by : Note<"'%0' required by '%1'">;
def note_ast_file_rebuild_required
    : Note<"please rebuild precompiled file '%0'">;
def note_module_cache_path : Note<
    "after modifying system headers, please delete the module cache at '%0'">;

def err_ast_file_targetopt_mismatch : Error<
    "precompiled file '%0' was compiled for the %1 '%2' but the current translation "
    "unit is being compiled for target '%3'">;
def err_ast_file_targetopt_feature_mismatch : Error<
    "%select{precompiled file '%1' was|current translation unit is}0 compiled with the target "
    "feature '%2' but the %select{current translation unit is|precompiled file '%1' was}0 "
    "not">;
def err_ast_file_langopt_mismatch : Error<"%0 was %select{disabled|enabled}1 in "
    "precompiled file '%3' but is currently %select{disabled|enabled}2">;
def err_ast_file_langopt_value_mismatch : Error<
  "%0 differs in precompiled file '%1' vs. current file">;
def err_ast_file_codegenopt_mismatch : Error<"%0 was %select{disabled|enabled}1 in "
  "precompiled file '%3' but is currently %select{disabled|enabled}2">;
def err_ast_file_codegenopt_value_mismatch
    : Error<"%0 differs in precompiled file '%1' vs. current file">;
def err_ast_file_diagopt_mismatch : Error<"%0 is currently enabled, but was not in "
  "the precompiled file '%1'">;
def err_ast_file_modulecache_mismatch : Error<"precompiled file '%2' was compiled with module cache "
  "path '%0', but the path is currently '%1'">;
def warn_pch_vfsoverlay_mismatch : Warning<
  "PCH was compiled with different VFS overlay files than are currently in use">,
  InGroup<DiagGroup<"pch-vfs-diff">>;
def note_pch_vfsoverlay_files : Note<"%select{PCH|current translation unit}0 has the following VFS overlays:\n%1">;
def note_pch_vfsoverlay_empty : Note<"%select{PCH|current translation unit}0 has no VFS overlays">;

def err_ast_file_version_too_old : Error<
    "%select{PCH|module|precompiled}0 file '%1' uses an older format that is no longer supported">;
def err_ast_file_version_too_new : Error<
    "%select{PCH|module|precompiled }0 file '%1' uses a newer format that cannot be read">;
def err_ast_file_different_branch : Error<
    "%select{PCH|module|precompiled}0 file '%1' built from a different branch (%2) than the compiler (%3)">;
def err_ast_file_with_compiler_errors : Error<
    "%select{PCH|module|precompiled}0 file '%1' contains compiler errors">;

def err_module_file_conflict : Error<
  "module '%0' is defined in both '%1' and '%2'">, DefaultFatal;
def err_ast_file_not_found : Error<
  "%select{PCH|module|precompiled}0 file '%1' not found%select{|: %3}2">, DefaultFatal;
def err_ast_file_out_of_date : Error<
  "%select{PCH|module|precompiled}0 file '%1' is out of date and "
  "needs to be rebuilt%select{|: %3}2">, DefaultFatal;
def err_ast_file_invalid : Error<
  "file '%1' is not a valid %select{PCH|module|precompiled}0 file: %2">, DefaultFatal;
def note_module_file_imported_by : Note<
  "imported by %select{|module '%2' in }1'%0'">;
def err_module_file_not_module : Error<
  "precompiled file '%0' was not built as a module">, DefaultFatal;
def err_module_file_missing_top_level_submodule : Error<
  "module file '%0' is missing its top-level submodule">, DefaultFatal;
def note_module_file_conflict : Note<
  "compiled from '%0' and '%1'">;

def remark_module_import : Remark<
  "importing module '%0'%select{| into '%3'}2 from '%1'">,
  ShowInSystemHeader,
  InGroup<ModuleImport>;

def err_imported_module_not_found : Error<
    "module '%0' in precompiled file '%1' %select{(imported by precompiled file '%2') |}4"
    "is not defined in any loaded module map file; "
    "maybe you need to load '%3'?">, DefaultFatal;
def note_imported_by_pch_module_not_found : Note<
    "consider adding '%0' to the header search path">;
def err_imported_module_modmap_changed : Error<
    "module '%0' %select{in|imported by}4 precompiled file '%1' found in a different module map file"
    " (%2) than when the importing precompiled file was built (%3)">, DefaultFatal;
def err_imported_module_relocated : Error<
    "module '%0' was built in directory '%1' but now resides in "
    "directory '%2'">, DefaultFatal;
def err_module_different_modmap : Error<
    "module '%0' %select{uses|does not use}1 additional module map '%2'"
    "%select{| not}1 used when the module was built">;

def err_ast_file_macro_def_undef : Error<
    "macro '%0' was %select{defined|undef'd}1 in the precompiled file '%2' but "
    "%select{undef'd|defined}1 on the command line">;
def err_ast_file_macro_def_conflict : Error<
    "definition of macro '%0' differs between the precompiled file '%3' ('%1') "
    "and the command line ('%2')">;
def err_ast_file_undef : Error<
    "%select{command line contains|precompiled file '%1' was built with}0 "
    "'-undef' but %select{precompiled file '%1' was not built with it|"
    "it is not present on the command line}0">;
def err_ast_file_pp_detailed_record : Error<
    "%select{command line contains|precompiled file '%1' was built with}0 "
    "'-detailed-preprocessing-record' but %select{precompiled file '%1' was not "
    "built with it|it is not present on the command line}0">;

def err_module_odr_violation_missing_decl : Error<
  "%q0 from module '%1' is not present in definition of %q2"
  "%select{ in module '%4'| provided earlier}3">, NoSFINAE;
def note_module_odr_violation_no_possible_decls : Note<
  "definition has no member %0">;
def note_module_odr_violation_possible_decl : Note<
  "declaration of %0 does not match">;

def err_module_odr_violation_different_instantiations : Error<
  "instantiation of %q0 is different in different modules">;

def warn_duplicate_module_file_extension : Warning<
  "duplicate module file extension block name '%0'">,
  InGroup<ModuleFileExtension>;

def warn_module_system_bit_conflict : Warning<
  "module file '%0' was validated as a system module and is now being imported "
  "as a non-system module; any difference in diagnostic options will be ignored">,
  InGroup<ModuleConflict>;

def warn_decls_in_multiple_modules : Warning<
  "declaration %0 is detected to be defined in multiple module units, first is from '%1' and second is from '%2'; "
  "the compiler may not be good at merging the definitions. ">,
  InGroup<DiagGroup<"decls-in-multiple-modules">>,
  DefaultIgnore;

def warn_module_file_mapping_mismatch
    : Warning<"loaded module file '%0' conflicts with imported file '%1'">,
      InGroup<DiagGroup<"module-file-mapping-mismatch">>,
      DefaultIgnore;

def err_failed_to_find_module_file : Error<
  "failed to find module file for module '%0'">;
} // let CategoryName

let CategoryName = "AST Serialization Issue" in {
def warn_module_uses_date_time : Warning<
  "%select{precompiled header|module}0 uses __DATE__ or __TIME__">,
  InGroup<DiagGroup<"pch-date-time">>;
def err_module_no_size_mtime_for_header : Error<
  "cannot emit module %0: %select{size|mtime}1 must be explicitly specified "
  "for missing header file \"%2\"">;
def err_module_unable_to_hash_content : Error<
  "failed to hash content for '%0' because memory buffer cannot be retrieved">;
} // let CategoryName
} // let Component


if {[catch {

# define run engine funtion
source [file join {C:/lscc/radiant/2023.1} scripts tcl flow run_engine.tcl]
# define global variables
global para
set para(gui_mode) 1
set para(prj_dir) "Z:/github/NeoObscura/software/fpga/combined"
# synthesize IPs
# synthesize VMs
# synthesize top design
file delete -force -- NeoObscura_impl_1.vm NeoObscura_impl_1.ldc
run_engine_newmsg synthesis -f "NeoObscura_impl_1_lattice.synproj"
run_postsyn [list -a iCE40UP -p iCE40UP5K -t SG48 -sp High-Performance_1.2V -oc Industrial -top -w -o NeoObscura_impl_1_syn.udb NeoObscura_impl_1.vm] "Z:/github/NeoObscura/software/fpga/combined/impl_1/NeoObscura_impl_1.ldc"

} out]} {
   runtime_log $out
   exit 1
}

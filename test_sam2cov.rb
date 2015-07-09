# Test in Ruby

# 1)
# Ask for valgrind
res = `command -v valgrind >/dev/null 2>&1 && VALGRIND=valgrind`
valgrind = ""
if $?.to_i == 0
  STDERR.puts "Using valgrind for more accurate testing."
  valgrind = "valgrind"
end

res = `make clean`
#puts res
#puts $1
raise res if $?.to_i != 0

res = `make`
raise res if $?.to_i != 0

res = `#{valgrind} ./sam2cov  -p both2tmp -s 0  -e 1 danRer7_s.fa.fai test.sam`
raise res if $?.to_i != 0
res = `diff both2tmpUnique.cov both2Unique.cov`
raise res if $?.to_i != 0 || res != ""

res = `diff both2NU.cov both2NU.cov`
raise res if $?.to_i != 0 || res != ""

res = `#{valgrind} ./sam2cov  -p prob -s 2 -e 1 Pf3D7_genome_one-line-seqs.fa.fai prob.sam`
raise res if $?.to_i != 0

res = `diff probUnique.cov prob2Unique.cov`
raise res if $?.to_i != 0 || res != ""

res = `diff probNU.cov prob2NU.cov`
raise res if $?.to_i != 0 || res != ""

res = `#{valgrind} ./sam2cov  -p gsnap2_tmp -s 0  -e 1 Pf3D7_genome_one-line-seqs.fa.fai gsnap2.sam`
raise res if $?.to_i != 0

res = `diff gsnap2_tmpNU.cov gsnap_tmpNU.cov`
raise res if $?.to_i != 0 || res != ""

res = `diff gsnap2_tmpUnique.cov probUnique.cov`
raise res if $?.to_i != 0 || res != ""

STDERR.puts "ALL TESTS FINISHED SUCCESSFULLY!"

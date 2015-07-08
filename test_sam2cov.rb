# Test in Ruby

# 1)
res = `make clean`
puts res
#puts $1
raise res if $?.to_i != 0

res = `make`
raise res if $?.to_i != 0

res = `valgrind ./sam2cov  -p both2tmp -s 0  -e 1 danRer7_s.fa.fai test.sam`
raise res if $?.to_i != 0
res = `diff both2tmpUnique.cov both2Unique.cov`
raise res if $?.to_i != 0 || res != ""

res = `diff both2NU.cov both2NU.cov`
raise res if $?.to_i != 0 || res != ""

res = `valgrind ./sam2cov  -p prob -s 2 -e 1 Pf3D7_genome_one-line-seqs.fa.fai prob.sam`
raise res if $?.to_i != 0

res = `diff probUnique.cov prob2Unique.cov`
raise res if $?.to_i != 0 || res != ""

res = `diff probNU.cov prob2NU.cov`
raise res if $?.to_i != 0 || res != ""

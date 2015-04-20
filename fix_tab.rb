File.open(ARGV[0]).each do |line|
  line.chomp!
  fields = line.split(" ")
  puts fields.join("\t")
end

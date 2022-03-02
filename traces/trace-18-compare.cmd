# Compare linux_sort and my merge sorting effiency.
option echo 0
option verbose 1

# linux sort
new
ih RAND 262144
time sort
free
new
ih RAND 262144
time sort
free
new
ih RAND 262144
time sort
free
new
ih RAND 262144
time sort
free
new
ih RAND 262144
time sort
free

# my sort
option list_sort 0
new
ih RAND 262144
time sort
free
new
ih RAND 262144
time sort
free
new
ih RAND 262144
time sort
free
new
ih RAND 262144
time sort
free
new
ih RAND 262144
time sort
free
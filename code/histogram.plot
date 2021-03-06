reset

# legendes
set title "Distribution des tirages"
set xlabel "valeurs"
set ylabel "freq"

# output
# ne fonctionne pas sous osx
#set term png
#set output "representation.png"

# params
n=1000	#number of intervals
max=400.	#max value
min=-200.	#min value
width=(max-min)/n	#interval width

# fonction histogramme
hist(x,width)=width*floor(x/width)+width/2.0

# borders
set xrange [min:max]
set yrange [0:]
set offset graph 0.05,0.05,0.05,0.0
set xtics min,(max-min)/5,max
set boxwidth width*0.9		# largeur fixe des batons
set style fill solid 0.5	#fillstyle
set tics out nomirror

plot "tirages.data" u (hist($1,width)):(1.0) smooth freq w boxes lc rgb"green" notitle

# sous linux ca permet de ne pas fermer la fenetre immediatement
pause -1

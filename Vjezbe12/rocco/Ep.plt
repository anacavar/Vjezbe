unset multiplot
reset
set encoding utf8

#POSTAVKE GRAFA: DIMENZIJE SLIKE, VELICINA FONTA, IME SLIKE
set term pngcairo background "#ffffff" font "Times-New-Roman,20pt" size 40.0cm,25.0cm fontscale 1.0 dl 2.0
set output 'Ep.png'

#STILOVI LINIJA; POZIV: ls BROJ
set style line  1 lt 1 lw 2 lc rgb "cyan"
set style line  2 lt 1 lw 2 lc rgb "blue"

set samples 10000
set bmargin 0.
set lmargin 0.
set rmargin 0.
set tmargin 0.

  set key top right #POSTAVLJA LEGENDU GORE DESNO
  unset key
  set origin 0.13,0.15 #POCETAK GRAFA ( u dimenzijama slike)
  set size   0.84,0.73 #VELICINA GRAFA (u dimenzijama slike)
    set title "VMC HO E_0 with respect to β"
    set xlabel "β"
    set ylabel "E_0[ħω]"
    set label "500 walkers, 500 steps, 200 blocks" at graph 0.05, graph 0.95
    set label "ψ(x) = e^{-βx^2}" at graph 0.05, graph 0.87
    set label "x^2 is in units of ħ/(mω)" at graph 0.25, graph 0.87
    set xrange [0.1:0.975]
    plot \
         "Ep.dat" w l ls 2

reset
set term GNUTERM
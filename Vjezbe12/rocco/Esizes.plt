unset multiplot
reset
set encoding utf8

#POSTAVKE GRAFA: DIMENZIJE SLIKE, VELICINA FONTA, IME SLIKE
set term pngcairo background "#ffffff" font "Times-New-Roman,20pt" size 40.0cm,25.0cm fontscale 1.0 dl 2.0
set output 'Esizes.png'

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
    set title "VMC HO Energy error with respect to steps per block"
    set xlabel "steps per block"
    set ylabel "E_{VMC}[ħω]-E_{exact}[ħω]"
    set label "100 walkers, 100 blocks" at graph 0.05, graph 0.95
    set label "E_{exact}[ħω] = 0.5" at graph 0.05, graph 0.88
    set label "E_{VMC}[ħω] = 0.5 +/- 0.0" at graph 0.05, graph 0.81
    set yrange [-0.01:0.01]
    plot "Esizes.dat" w l ls 2

reset
set term GNUTERM
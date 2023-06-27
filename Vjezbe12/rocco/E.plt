unset multiplot
reset
set encoding utf8

#POSTAVKE GRAFA: DIMENZIJE SLIKE, VELICINA FONTA, IME SLIKE
set term pngcairo background "#ffffff" font "Times-New-Roman,20pt" size 40.0cm,25.0cm fontscale 1.0 dl 2.0
set output 'E.png'

#STILOVI LINIJA; POZIV: ls BROJ
set style line  1 lt 1 lw 2 lc rgb "cyan"
set style line  2 lt 1 lw 2 lc rgb "blue"

set samples 10000
set bmargin 0.
set lmargin 0.
set rmargin 0.
set tmargin 0.

  set key top right #POSTAVLJA LEGENDU GORE DESNO
  set origin 0.13,0.15 #POCETAK GRAFA ( u dimenzijama slike)
  set size   0.84,0.83 #VELICINA GRAFA (u dimenzijama slike)
    set xlabel "blocks / 500 steps"
    set ylabel "{/Times-New-Roman-Italic E}[ħω]"
    set label "500 walkers" at graph 0.05, graph 0.95
    set label "VMC (β=0.5)" at graph 0.05, graph 0.87
    plot \
         "E.dat" u 1:2 w l ls 1 ti "<{/Times-New-Roman-Italic E}_b>", \
         "E.dat" u 1:3 w l ls 2 ti "<{/Times-New-Roman-Italic E}>"

reset
set term GNUTERM

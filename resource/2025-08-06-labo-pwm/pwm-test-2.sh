la -p 2-5 enable
sleep 1000
pwm 2-5 func:pwm disable freq:100 counter:0 phase-correct:true
pwm2 duty:.2; pwm3 duty:.4; pwm4 duty:.6; pwm5 duty:.8
pwm 2-9 enable
sleep 2000
la print

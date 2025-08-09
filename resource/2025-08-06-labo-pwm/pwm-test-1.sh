la -p 2,3,4,5 enable
pwm 2,3,4,5 func:pwm disable freq:100 counter:0
pwm2 duty:.2
pwm3 duty:.4
pwm4 duty:.6
pwm5 duty:.8
pwm 2,3,4,5 enable
sleep 2000
la print

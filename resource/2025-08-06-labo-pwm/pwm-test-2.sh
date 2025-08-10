reset pwm
pwm 2-5 func:pwm phase-correct:true freq:100 counter:0
la -p 2-5 --samplers:4 enable
sleep 1000
pwm2 duty:.2; pwm3 duty:.4; pwm4 duty:.6; pwm5 duty:.8
pwm 2-5 enable
sleep 4000
pwm 2-5 disable
la print --events:80

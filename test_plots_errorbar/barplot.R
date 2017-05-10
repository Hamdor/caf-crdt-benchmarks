## Run 1
rx10_1 <- scan("cvrdt/1/rx_out_10.txt") / 1000000
tx10_1 <- scan("cvrdt/1/tx_out_10.txt") / 1000000
rx20_1 <- scan("cvrdt/1/rx_out_20.txt") / 1000000
tx20_1 <- scan("cvrdt/1/tx_out_20.txt") / 1000000
rx40_1 <- scan("cvrdt/1/rx_out_40.txt") / 1000000
tx40_1 <- scan("cvrdt/1/tx_out_40.txt") / 1000000
## Run 2
rx10_2 <- scan("cvrdt/2/rx_out_10.txt") / 1000000
tx10_2 <- scan("cvrdt/2/tx_out_10.txt") / 1000000
rx20_2 <- scan("cvrdt/2/rx_out_20.txt") / 1000000
tx20_2 <- scan("cvrdt/2/tx_out_20.txt") / 1000000
rx40_2 <- scan("cvrdt/2/rx_out_40.txt") / 1000000
tx40_2 <- scan("cvrdt/2/tx_out_40.txt") / 1000000
## Run 3
rx10_3 <- scan("cvrdt/3/rx_out_10.txt") / 1000000
tx10_3 <- scan("cvrdt/3/tx_out_10.txt") / 1000000
rx20_3 <- scan("cvrdt/3/rx_out_20.txt") / 1000000
tx20_3 <- scan("cvrdt/3/tx_out_20.txt") / 1000000
rx40_3 <- scan("cvrdt/3/rx_out_40.txt") / 1000000
tx40_3 <- scan("cvrdt/3/tx_out_40.txt") / 1000000
## Run 4
rx10_4 <- scan("cvrdt/4/rx_out_10.txt") / 1000000
tx10_4 <- scan("cvrdt/4/tx_out_10.txt") / 1000000
rx20_4 <- scan("cvrdt/4/rx_out_20.txt") / 1000000
tx20_4 <- scan("cvrdt/4/tx_out_20.txt") / 1000000
rx40_4 <- scan("cvrdt/4/rx_out_40.txt") / 1000000
tx40_4 <- scan("cvrdt/4/tx_out_40.txt") / 1000000
## Run 5
rx10_5 <- scan("cvrdt/5/rx_out_10.txt") / 1000000
tx10_5 <- scan("cvrdt/5/tx_out_10.txt") / 1000000
rx20_5 <- scan("cvrdt/5/rx_out_20.txt") / 1000000
tx20_5 <- scan("cvrdt/5/tx_out_20.txt") / 1000000
rx40_5 <- scan("cvrdt/5/rx_out_40.txt") / 1000000
tx40_5 <- scan("cvrdt/5/tx_out_40.txt") / 1000000
## Run 6
rx10_6 <- scan("cvrdt/6/rx_out_10.txt") / 1000000
tx10_6 <- scan("cvrdt/6/tx_out_10.txt") / 1000000
rx20_6 <- scan("cvrdt/6/rx_out_20.txt") / 1000000
tx20_6 <- scan("cvrdt/6/tx_out_20.txt") / 1000000
rx40_6 <- scan("cvrdt/6/rx_out_40.txt") / 1000000
tx40_6 <- scan("cvrdt/6/tx_out_40.txt") / 1000000
## Run 7
rx10_7 <- scan("cvrdt/7/rx_out_10.txt") / 1000000
tx10_7 <- scan("cvrdt/7/tx_out_10.txt") / 1000000
rx20_7 <- scan("cvrdt/7/rx_out_20.txt") / 1000000
tx20_7 <- scan("cvrdt/7/tx_out_20.txt") / 1000000
rx40_7 <- scan("cvrdt/7/rx_out_40.txt") / 1000000
tx40_7 <- scan("cvrdt/7/tx_out_40.txt") / 1000000
## Run 8
rx10_8 <- scan("cvrdt/8/rx_out_10.txt") / 1000000
tx10_8 <- scan("cvrdt/8/tx_out_10.txt") / 1000000
rx20_8 <- scan("cvrdt/8/rx_out_20.txt") / 1000000
tx20_8 <- scan("cvrdt/8/tx_out_20.txt") / 1000000
rx40_8 <- scan("cvrdt/8/rx_out_40.txt") / 1000000
tx40_8 <- scan("cvrdt/8/tx_out_40.txt") / 1000000
## Run 9
rx10_9 <- scan("cvrdt/9/rx_out_10.txt") / 1000000
tx10_9 <- scan("cvrdt/9/tx_out_10.txt") / 1000000
rx20_9 <- scan("cvrdt/9/rx_out_20.txt") / 1000000
tx20_9 <- scan("cvrdt/9/tx_out_20.txt") / 1000000
rx40_9 <- scan("cvrdt/9/rx_out_40.txt") / 1000000
tx40_9 <- scan("cvrdt/9/tx_out_40.txt") / 1000000
## Run 10
rx10_10 <- scan("cvrdt/10/rx_out_10.txt") / 1000000
tx10_10 <- scan("cvrdt/10/tx_out_10.txt") / 1000000
rx20_10 <- scan("cvrdt/10/rx_out_20.txt") / 1000000
tx20_10 <- scan("cvrdt/10/tx_out_20.txt") / 1000000
rx40_10 <- scan("cvrdt/10/rx_out_40.txt") / 1000000
tx40_10 <- scan("cvrdt/10/tx_out_40.txt") / 1000000

rx_10 <- c(rx10_1, rx10_2 ,rx10_3 ,rx10_4, rx10_5, rx10_6 ,rx10_7,rx10_8, rx10_9 ,rx10_10)
rx_10_sd <- sd(rx_10)
rx_10_mean <- mean(rx_10)

tx_10 <- c(tx10_1, tx10_2 ,tx10_3 ,tx10_4, tx10_5, tx10_6 ,tx10_7,tx10_8, tx10_9 ,tx10_10)
tx_10_sd <- sd(tx_10)
tx_10_mean <- mean(tx_10)

rx_20 <- c(rx20_1, rx20_2 ,rx20_3 ,rx20_4, rx20_5, rx20_6 ,rx20_7,rx20_8, rx20_9 ,rx20_10)
rx_20_sd <- sd(rx_20)
rx_20_mean <- mean(rx_20)

tx_20 <- c(tx20_1, tx20_2 ,tx20_3 ,tx20_4, tx20_5, tx20_6 ,tx20_7,tx20_8, tx20_9 ,tx20_10)
tx_20_sd <- sd(tx_20)
tx_20_mean <- mean(tx_20)

rx_40 <- c(rx40_1, rx40_2 ,rx40_3 ,rx40_4, rx40_5, rx40_6 ,rx40_7,rx40_8, rx40_9 ,rx40_10)
rx_40_sd <- sd(rx_40)
rx_40_mean <- mean(rx_40)

tx_40 <- c(tx40_1, tx40_2 ,tx40_3 ,tx40_4, tx40_5, tx40_6 ,tx40_7,tx40_8, tx40_9 ,tx40_10)
tx_40_sd <- sd(tx_40)
tx_40_mean <- mean(tx_40)

column1 <- c(rep("10", 2), rep("20", 2), rep("40", 2))
column2 <- gl(2, 1, 6, c("TX", "RX"))
column3 <- c(tx_10_mean, rx_10_mean, tx_20_mean, rx_20_mean, tx_40_mean, rx_40_mean)
column4 <- c(tx_10_sd, rx_10_sd, tx_20_sd, rx_20_sd, tx_40_sd, rx_40_sd)

d <- data.frame(column1=column1, column2=column2, column3=column3, column4=column4)

require(ggplot2)
ggplot(d, aes(x=column1, y=column3, fill=column2)) +
  geom_bar(stat="identity", position=position_dodge()) +
  scale_x_discrete("Anzahl Knoten [#]") +
  scale_y_continuous("Datenaufkommen [MBytes]") +
  guides(fill=guide_legend(title=NULL)) +
  geom_errorbar(aes(ymin=column3-column4, ymax=column3+column4), width=.2, position=position_dodge(.9))

rx10 <- scan("rx_out_10.txt") / 1000000
tx10 <- scan("tx_out_10.txt") / 1000000
rx20 <- scan("rx_out_20.txt") / 1000000
tx20 <- scan("tx_out_20.txt") / 1000000
rx40 <- scan("rx_out_40.txt") / 1000000
tx40 <- scan("tx_out_40.txt") / 1000000

column1 <- c(rep("10", 2), rep("20", 2), rep("40", 2))
column2 <- gl(2, 1, 6, c("TX", "RX"))
column3 <- c(tx10, rx10, tx20, rx20, tx40, rx40)

d <- data.frame(column1=column1, column2=column2, column3=column3)

require(ggplot2)
ggplot(d, aes(x=column1, y=column3, fill=column2)) + geom_bar(stat="identity", position=position_dodge()) + scale_x_discrete("Anzahl Knoten [#]") + scale_y_continuous("Datenaufkommen [MBytes]") + guides(fill=guide_legend(title=NULL))

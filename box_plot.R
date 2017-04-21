data<-data.frame(read.table("conv/convergence_1000_100.txt"),
                 read.table("conv/convergence_1000_200.txt"),
                 read.table("conv/convergence_1000_400.txt"),
                 read.table("conv/convergence_1000_800.txt"))

boxplot(data, ylab ="Zeitdifferenz [usec]", xlab ="Anzahl an Abonnenten [#]", names = c("100", "200", "400", "800"), log="y")
grid(col="lightgray", lty="dotted")

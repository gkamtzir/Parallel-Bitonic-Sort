CC=gcc
SDIR=./source
BDIR=./bin


all: $(BDIR)/pthreadsRec $(BDIR)/cilkRec $(BDIR)/cilkImp $(BDIR)/mpRec $(BDIR)/mpImp

$(SDIR)/pthreadsRec.o: $(SDIR)/pthreadsRec.c
	$(CC) -c $^ -o $@ -O3 -pthread

$(SDIR)/cilkRec.o: $(SDIR)/cilkRec.c
	icc -c $^ -o $@ -O3

$(SDIR)/cilkImp.o: $(SDIR)/cilkImp.c
	icc -c $^ -o $@ -O3

$(SDIR)/mpRec.o: $(SDIR)/mpRec.c
	$(CC) -c $^ -o $@ -O3 -fopenmp

$(SDIR)/mpImp.o: $(SDIR)/mpImp.c
	$(CC) -c $^ -o $@ -O3 -fopenmp

$(BDIR)/pthreadsRec: $(SDIR)/pthreadsRec.o
	$(CC) $^ -o $@ -O3 -pthread

$(BDIR)/cilkRec: $(SDIR)/cilkRec.o
	icc $^ -O3 -o $@
	
$(BDIR)/cilkImp: $(SDIR)/cilkImp.o
	icc $^ -O3 -o $@
	
$(BDIR)/mpRec: $(SDIR)/mpRec.o
	$(CC) $^ -O3 -fopenmp -o $@

$(BDIR)/mpImp: $(SDIR)/mpImp.o
	$(CC) $^ -O3 -fopenmp -o $@

clean:
	rm $(SDIR)/*.o

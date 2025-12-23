CC = gcc
CFLAGS = -Wall -Wextra -O2 `pkg-config --cflags libcurl`
LDFLAGS = `pkg-config --libs libcurl`
SRCDIR = src
BINDIR = bin
OBJS = $(SRCDIR)/main.o $(SRCDIR)/agent.o $(SRCDIR)/tools.o $(SRCDIR)/http_client.o

all: $(BINDIR) $(BINDIR)/agent

$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/agent: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRCDIR)/main.o: $(SRCDIR)/main.c $(SRCDIR)/agent.h $(SRCDIR)/tools.h

$(SRCDIR)/agent.o: $(SRCDIR)/agent.c $(SRCDIR)/agent.h $(SRCDIR)/http_client.h $(SRCDIR)/tools.h

$(SRCDIR)/tools.o: $(SRCDIR)/tools.c $(SRCDIR)/tools.h $(SRCDIR)/http_client.h

$(SRCDIR)/http_client.o: $(SRCDIR)/http_client.c $(SRCDIR)/http_client.h

clean:
	rm -rf $(BINDIR)/*.o $(SRCDIR)/*.o $(BINDIR)

.PHONY: all clean

TARGET=loggen

CC=gcc
LD=gcc

CFLAGS= -Wall
LDFLAGS=

SRCS= markov.c loggen.c

OBJS= $(SRCS:.c=.o)

DEBUG=1
ifdef DEBUG
  CFLAGS += -g -ggdb3 -O0
else
  CFLAGS += -O2
endif

ifeq ("$(origin V)", "command line")
  BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
  BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),1)
  quiet =_
  Q =
else
  quiet=quiet_
  Q = @
endif

print=if [ $(BUILD_VERBOSE) = 0 ]; then echo "$(1)"; fi

quiet_cmd_cc="CC $@"
      cmd_cc=$(CC) $(CFLAGS) -c -o $@ $<

quiet_cmd_ld="LD $@"
      cmd_ld=$(CC) $(LDFLAGS) -o $@ $^

quiet_cmd_clean="CLEAN"
      cmd_clean=rm -f $(TARGET) *.o .depend

quiet_cmd_depend="DEPEND"
      cmd_depend=[ -e $@ ] && rm $@ ; \
	         for file in *.c; do \
			$(CC) -MM -MT "$${file%.c}.o $@" $(CFLAGS) $(INCS) $$file >>$@ 2>/dev/null; \
		 done

.PHONY: all dep

all: dep $(TARGET)

#ifneq ($(MAKECMDGOALS),clean)
# -include .depend
#endif

$(TARGET): $(OBJS)
	@$(call print,$($(quiet)cmd_ld))
	$(Q)$(cmd_ld)

%.o : %.c
	@$(call print,$($(quiet)cmd_cc))
	$(Q)$(cmd_cc)

dep: .depend

.depend:
	@$(call print,$($(quiet)cmd_depend))
	$(Q)$(cmd_depend)

clean:
	@$(call print,$($(quiet)cmd_clean))
	$(Q)$(cmd_clean)

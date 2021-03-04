APP_DIR=./app

APP_C_SOURCES = \
$(APP_DIR)/app.c \
$(APP_DIR)/sys_timer.c \
$(APP_DIR)/circ_buffer.c \
$(APP_DIR)/event_dispatcher.c \
$(APP_DIR)/mainloop_timer.c \
$(APP_DIR)/soft_timer.c \
$(APP_DIR)/shell.c \
$(APP_DIR)/shell_if_usb.c \
$(APP_DIR)/misc.c \
$(APP_DIR)/stepper.c


C_SOURCES += $(APP_C_SOURCES)
CFLAGS += -I$(APP_DIR)

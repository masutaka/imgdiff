ifeq ($(shell uname), SunOS)
CC       = /usr/local/bin/gcc
ENDIAN   = BIGENDIAN
INCLUDES = -I/usr/local/include
LIBS     = /usr/local/lib/libtiff.a /usr/local/lib/libjpeg.a /usr/local/lib/libz.a -lm -lc
JPEG     = ENABLE_JPEG
else
CC       = gcc
ENDIAN   = LITTLEENDIAN
INCLUDES = 
LIBS     = -ltiff
JPEG     = UNABLE_JPEG
endif

CFLAGS   = -D$(ENDIAN) -D$(JPEG) -Wall -Wextra -Werror -g
LDFLAGS  = 
TARGET   = imgdiff
OBJS     = id_main.o id_tiff.o id_pnm.o id_jpeg.o id_diff.o id_diff_image.o

# For Release
CFLAGS   = -D$(ENDIAN) -D$(JPEG) -O3 -Wall -Wextra -Werror
LDFLAGS  = -s

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

id_main.o: id_std.h id_readme.h id_tiff.h id_pnm.h id_jpeg.h id_diff.h
id_tiff.o: id_std.h id_tiff.h
id_pnm.o:  id_std.h id_pnm.h
id_jpeg.o: id_std.h id_jpeg.h
id_diff.o: id_std.h id_diff.h id_diff_image.h
id_diff_image.o: id_std.h id_diff_image.h

clean:
	-rm -f $(TARGET) $(TARGET).exe $(OBJS)

# For flymake-mode on Emacs
check-syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(CHK_SOURCES)

### End of file

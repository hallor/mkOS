%.o: %.c
	$(CC) $(DEPFLAGS) $(CFLAGS) -D__THIS_FILE__='"$(<F)"' -o $@ -c $<

%.o: %.cpp
	$(CXX) $(DEPFLAGS) $(CPPFLAGS) -D__THIS_FILE__='"$(<F)"' -o $@ -c $<

%.o: %.S
	$(CC) $(DEPFLAGS) $(ASFLAGS) -D__THIS_FILE__='"$(<F)"' -o $@ -c $<

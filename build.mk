%.o: %.c
	$(CC) $(DEPFLAGS) $(CFLAGS) -o $@ -c $<

%.o: %.cpp
	$(CXX) $(DEPFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.S
	$(CC) $(DEPFLAGS) $(ASFLAGS) -o $@ -c $<

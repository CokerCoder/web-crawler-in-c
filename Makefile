all:
	@mkdir build
	@cd build ; cmake ..
	$(MAKE) -C build all
	@cd build; mv ./crawler ../


clean:
	-rm -rf build/
	-rm -f ./crawler
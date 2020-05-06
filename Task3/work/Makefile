
default: info


help:
	less README.tjk

compile:
	javac -nowarn *.java

run:
	java Scheduling scheduling.conf

run2:
	java Scheduling scheduling2Processes.conf

run5:
	java Scheduling scheduling5Processes.conf

run10:
	java Scheduling scheduling10Processes.conf

setup:
	./setUp

info:
	@echo ""
	@echo "Use 'make' with one argument:"
	@echo ""
	@echo "  make help"
	@echo "  make compile"
	@echo "  make run"
	@echo ""

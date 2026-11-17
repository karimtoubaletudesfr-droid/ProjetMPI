CXX = mpic++
CXXFLAGS = -std=c++11

WORLD ?= fichierMonde/monde1.txt
TOLERANCE ?= 0.0
NP ?= 3

DIR = repartition_de_base
PARALLELE_SRC = parallele/main.cpp parallele/src/habitant.cpp parallele/src/ile.cpp parallele/src/monde.cpp

.PHONY: build-parallele run-parallele

format:
	@find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format -i {} +

test-partition:
	$(CXX) $(CXXFLAGS) $(DIR)/main_test_partition.cpp $(DIR)/graphe.cpp $(DIR)/weightedGreedyGraphPartitioning/weightedGreedyGraphPartitioning.cpp $(DIR)/roundRobin/roundRobinPartitioning.cpp -o test_partition

run-partition: test-partition
	mpirun -np $(NP) ./test_partition $(WORLD) $(TOLERANCE)

benchmark:
	/usr/bin/time -f "Temps total : %e secondes" $(MAKE) run-partition NP=$(NP) WORLD=$(WORLD) TOLERANCE=$(TOLERANCE)

benchmark-roundrobin:
	$(CXX) $(CXXFLAGS) $(DIR)/main_test_partition.cpp $(DIR)/graphe.cpp $(DIR)/roundRobin/roundRobinPartitioning.cpp -o test_partition
	@echo "Début du test : $$(date '+%H:%M:%S')"
	@/usr/bin/time -f "Durée totale : %e secondes" mpirun -np $(NP) ./test_partition $(WORLD) $(TOLERANCE)
	@echo "Fin du test : $$(date '+%H:%M:%S')"
clean:
	rm -f test_partition monde_parallele

build-parallele:
	$(CXX) $(CXXFLAGS) -Iparallele/include $(PARALLELE_SRC) -o monde_parallele

run-parallele: build-parallele
	./monde_parallele

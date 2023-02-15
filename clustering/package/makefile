CC=gcc
CFLAGS=--shared -O3
SRC=TBAG/src/agglomerative.c
OBJ=agglomerative.o
LIB=TBAG/lib/trajectory_clustering.dll
TARGET=agglomerative
PY38=C:\\Users\\Ofek\\AppData\\Local\\Programs\\Python\\Python38\\python.exe
PY310=C:\\Users\\Ofek\\AppData\\Local\\Programs\\Python\\Python310\\python.exe
PY311=C:\\Users\\Ofek\\AppData\\Local\\Programs\\Python\\Python311\\python.exe
PY38_DIST=dist/TBAG-0.0.4-cp38-cp38-win_amd64.whl
PY310_DIST=dist/TBAG-0.0.4-cp310-cp310-win_amd64.whl
PY311_DIST=dist/TBAG-0.0.4-cp311-cp311-win_amd64.whl
PY_DIST=dist/TBAG-0.0.4-py3-none-any.whl

TBAGpy311: TBAGpy310
	$(PY311) -m build --wheel
	mv $(PY_DIST) $(PY311_DIST) 

TBAGpy310: TBAGpy38
	$(PY310) -m build --wheel
	mv $(PY_DIST) $(PY310_DIST) 

TBAGpy38: $(TARGET)
	$(PY38) -m build --wheel
	mv $(PY_DIST) $(PY38_DIST) 

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(LIB)

$(OBJ): $(SRC)
	$(CC) -c $(CFLAGS) $(SRC)

clean:
	$(RM) $(LIB) $(OBJ) dist/*
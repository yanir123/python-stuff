# TBAG

Trajectory Based Agglomerative Grouping or TBAG, is a clustring algorithem that elevates the classical algorithm with the use of phyisical nature of projectiles.

The algorithm is similar to regular agglomerative clustering in the sense that it is merging two closest clusters in a 'bottom up' method, but it also checks continuity between them.

## Installation

First you need to install the library for your python version and operating system:

`python -m pip install TBAG-0.0.1-cp<v>-cp<v>-<os>`

where \<v> should be replaced with the python version major
and the \<os> should be replaced with the operating system you are publishing on.

for example:

`python -m pip install TBAG-0.0.1-cp310-cp310-win_amd64`

### Not yet supported versions or environments

If your operating system of python version does not have a corresponding `.whl` file please contact `���� �� ���`.

### Currently supported versions

- `3.8`
- `3.10`
- `3.11`

### Currently supported operating systems

- `win_amd64`
- `manylinux_2_17_x86_64` to `manylinux2014_x86_64`

## Usage

We tried to make the usage as similar to the sklean api as possible.

First you need to import the package:

`from TBAG import TBAG`

The model constructor has several parameters with default values:

```
eps: float - maximum distance between clusters default=200,
alpha: float - maximum speed differnece between new point and last `window` points in cluster default=10,
time_eps: float - maximum time difference between clusters default=np.inf,
speed_eps: float - maximum speed differnece between new point and last `window` points in cluster default=300,
window: int - number of points in cluster to calculate the alpha on default=4
```

Now you can create an instance with the parameters:

`model = TBAG(eps=100, alpha=3000, max_speed=300, min_speed=0, window=4)`

The fit part of the model only sets the data and organizes it.
The fit receives parameters on the data itself so that it can work with it:

```
data: pd.DataFrame - the dataframe holding the points (it should be sorted by time ascending),
lat_col: str - the name of the lat column in the dataframe default='lat',
lon_col: str - the name of the lon column in the dataframe default='lon',
alt_col: str - the name of the alt column in the dataframe default='alt',
timestamp_col: str - the name of the timestamp column in the dataframe default='timestamp',
cast_to_radians: bool - flag that should be set to True if data is in degrees and not radians default=False
```

for example:

`model.fit(plots, lat_col='Lat', lon_col='Lon', alt_col='Alt', timestamp_col='epoch')`

Finally we can predict the clusters and get our result:

`res = model.predict()`

Note, this can be shortend using the `fit_predict` method like this:

`res = model.fit_predict(plots, lat_col='Lat', lon_col='Lon', alt_col='Alt', timestamp_col='epoch')`

## Build from source

If you are in DE_Inferno or if you have `Celiac disease` and you want to Upgrade/Rebuild the package you can use the following steps:

### Build form `C` code

First navigate to the `C` code directory in:

```
package
└───build
└───dist
└───TBAG
|   └───lib
|   └───src
|   |   |
|   |   |   agglomerative.c
|   |   |   agglomerative.h
|   |   |   main.c
|   |   __init__.py
|   |   TrajectoryClustering.py
└───TBAG.egg-info
|   
│   README.md
│   setup.py    
```

And then run the comman:

Windows: 

`gcc -O3 -shared agglomerative.c -o ..\lib\trajectory_clustering.dll`

or for Linux:

`gcc -O3 -shared agglomerative.c -o ..\lib\trajectory_clustering.so`

### Build `whl` file

After building the library you can go back to the package dir and build the whl file.

But before that please make sure that the python version in the `setup.py` file, for example:

`python_requires='>=3.11'`

Then we can run the command:

`python -m build --wheel`

Or just run `make` if you have it installed.

Please make sure that you are using the right python version when building the distribution.

Your new whl file will be ready in the dist directory.

I hope you enjoy this library like I suffered writing this readme :).

### Author's note

A special thank you to `Kobi Swisa` A.K.A `KS Hamutag`. 
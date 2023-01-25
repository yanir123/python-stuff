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

If your operating system of python version does not have a corresponding `.whl` file please contact `עומר אר הדר`.

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
alpha: float - maximum angle variance between clusters default=2500,
time_eps: float - maximum time difference between clusters default=np.inf,
max_speed: float - maximum speed default=300,
min_speed: float - minmum speed dafult=100,
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
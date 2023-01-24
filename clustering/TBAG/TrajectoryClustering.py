import datetime as dt
import numpy as np
import pandas as pd
import ctypes
from numpy import ctypeslib as npct


class TBAG(object):
    """
        Api class to use the trajectory algorithm
    """

    def __init__(self,
                 eps: float = 200,
                 alpha: float = 2500,
                 time_eps: float = np.inf,
                 max_speed: float = 300,
                 min_speed: float = 100,
                 window: int = 4) -> None:
        try:
            self.c_trajectory_clustering_function = npct.load_library(
                r'trajectory_clustering.dll',
                r'C:\Users\Ofek\Documents\python-stuff\clustering\TBAG\lib'  # TODO: change to relative path
            ).agglomerative_clustering
        except OSError:
            raise OSError(
                'Unable to locate the library. please contact sharoni(הגאון מהטכניון)'
            )

        self.array_2d_double_type = npct.ndpointer(dtype=np.uintp,
                                                   ndim=1,
                                                   flags='CONTIGUOUS')
        self.array_1d_int_type = npct.ndpointer(dtype=np.int32,
                                                ndim=1,
                                                flags='CONTIGUOUS')
        self.c_trajectory_clustering_function.argtypes = [
            self.array_2d_double_type, ctypes.c_size_t, ctypes.c_double,
            ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double,
            ctypes.c_size_t, self.array_1d_int_type
        ]
        self.c_trajectory_clustering_function.restype = None

        self.eps = ctypes.c_double(eps)
        self.alpha = ctypes.c_double(alpha)
        self.time_eps = ctypes.c_double(time_eps)
        self.max_speed = ctypes.c_double(max_speed)
        self.min_speed = ctypes.c_double(min_speed)
        self.window = ctypes.c_size_t(window)

    def fit(self,
            data: np.ndarray | pd.DataFrame,
            lat_col: str = 'lat',
            lon_col: str = 'lon',
            alt_col: str = 'alt',
            timestamp_col: str = 'timestamp',
            cast_to_radians: bool = False) -> None:
        TBAG.check_params(data, lat_col, lon_col, alt_col, timestamp_col)

        if isinstance(data, pd.DataFrame):
            self.data = np.ascontiguousarray(
                data[[lat_col, lon_col, alt_col,
                      timestamp_col]].to_numpy().astype('float64'))

        if cast_to_radians:
            self.data[:, :2] = np.deg2rad(self.data[:, :2])

        self.len = self.data.shape[0]

        self.data = (
            self.data.__array_interface__['data'][0] +
            np.arange(self.data.shape[0]) * self.data.strides[0]).astype(
                np.uintp)
        self.c_len = ctypes.c_size_t(self.len)

    def predict(self) -> np.array:
        res = np.zeros(self.len, dtype='int32')

        self.c_trajectory_clustering_function(self.data, self.c_len, self.eps,
                                              self.time_eps, self.alpha,
                                              self.max_speed, self.min_speed,
                                              self.window, res)

        return res

    def fit_predict(self,
                    data: np.ndarray | pd.DataFrame,
                    lat_col: str = 'lat',
                    lon_col: str = 'lon',
                    alt_col: str = 'alt',
                    timestamp_col: str = 'timestamp',
                    cast_to_radians: bool = False) -> np.array:
        self.fit(data, lat_col, lon_col, alt_col, timestamp_col,
                 cast_to_radians)
        return self.predict()

    @staticmethod
    def check_params(data, lat_col, lon_col, alt_col, timestamp_col):
        if isinstance(data, pd.DataFrame):
            for col in (lat_col, lon_col, alt_col, timestamp_col):
                if col not in data.columns:
                    raise ValueError(f'Column {col} is not in dataframe!')
        elif not isinstance(data, np.ndarray):
            raise ValueError(
                'Parameter data must be either numpy.ndarray or pandas.DataFrame'
            )

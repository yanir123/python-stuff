import numpy as np
import pandas as pd
import sys
import ctypes
import os
from numpy import ctypeslib as npct


class TBAG(object):

    def __init__(self,
                 eps: float = 200,
                 alpha: float = 2500,
                 time_eps: float = np.inf,
                 max_speed: float = 300,
                 min_speed: float = 100,
                 window: int = 4) -> None:
        """Class that is used as an API to run the underlying c code

        Parameters
        ----------
        eps : float, optional
            maximum distance in `meters` between the end of one cluster and the start of another, by default 200
        alpha : float, optional
            maximum variance in the azimuth between the `window` last points and the `window` first points in the combining clusters, by default 2500
        time_eps : float, optional
            maximum time diferrence when the first cluster ends and the second starts, by default np.inf
        max_speed : float, optional
            maximum speed allowed, by default 300
        min_speed : float, optional
            minimum speed allowed, by default 100
        window : int, optional
            the number of points to take from the start and end of the combining clusters, by default 4

        Raises
        ------
        OSError
            If the c library was not found
        """
        try:
            self.c_trajectory_clustering_function = npct.load_library(
                'trajectory_clustering',
                os.path.dirname(sys.modules['TBAG'].__file__)
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
        """Function that will fit the data in the model

        Parameters
        ----------
        data : np.ndarray | pd.DataFrame
            either numpy array or pandas dataframe containing the data.\n
            if `np.ndarray` it should be of size (n, 4) where n is the number of data points and 4 representing (lat, lon, alt, timestamp).\n
            if `pd.DataFrame` the *_col parameters represent the keys that hold the relevant data.
        lat_col : str, optional
            column name of the latitude value, by default 'lat'
        lon_col : str, optional
            column name of the longitude value, by default 'lon'
        alt_col : str, optional
            column name of the altitude value, by default 'alt'
        timestamp_col : str, optional
            column name of the timestamp value, by default 'timestamp'
        cast_to_radians : bool, optional
            condition that indicates if the data should be casted to radians, by default False
        """
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
        """Function to cluster the values into groups

        Returns
        -------
        np.array
            array containing the cluster ids
        """
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
        """Function that will fit the data in the model and predict on it

        Parameters
        ----------
        data : np.ndarray | pd.DataFrame
            either numpy array or pandas dataframe containing the data.\n
            if `np.ndarray` it should be of size (n, 4) where n is the number of data points and 4 representing (lat, lon, alt, timestamp).\n
            if `pd.DataFrame` the *_col parameters represent the keys that hold the relevant data.
        lat_col : str, optional
            column name of the latitude value, by default 'lat'
        lon_col : str, optional
            column name of the longitude value, by default 'lon'
        alt_col : str, optional
            column name of the altitude value, by default 'alt'
        timestamp_col : str, optional
            column name of the timestamp value, by default 'timestamp'
        cast_to_radians : bool, optional
            condition that indicates if the data should be casted to radians, by default False

        Returns
        -------
        np.array
            array containing the cluster ids
        """
        self.fit(data, lat_col, lon_col, alt_col, timestamp_col,
                 cast_to_radians)
        return self.predict()

    @staticmethod
    def check_params(data: np.ndarray | pd.DataFrame, lat_col: str,
                     lon_col: str, alt_col: str, timestamp_col: str):
        """Function to check if parameters are valid

        Parameters
        ----------
        `data` : np.ndarray | pd.DataFrame
            either numpy array or pandas dataframe containing the data.\n
            if `np.ndarray` it should be of size (n, 4) where n is the number of data points and 4 representing (lat, lon, alt, timestamp).\n
            if `pd.DataFrame` the *_col parameters represent the keys that hold the relevant data.
        `lat_col` : str
            colomn name of the latitude value
        `lon_col` : str
            column name of the longitude value
        `alt_col` : str
            column name of the altitude value
        `timestamp_col` : str
            column name o te timestamp value

        Raises
        ------
        `ValueError`
            if any given column name is not in the dataframe
        `ValueError`
            if given data parameter is not np.ndarray or pd.DataFrame
        """
        if isinstance(data, pd.DataFrame):
            for col in (lat_col, lon_col, alt_col, timestamp_col):
                if col not in data.columns:
                    raise ValueError(f'Column {col} is not in dataframe!')
        elif not isinstance(data, np.ndarray):
            raise ValueError(
                'Parameter data must be either numpy.ndarray or pandas.DataFrame'
            )

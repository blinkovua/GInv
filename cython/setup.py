from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = [
    Extension("*", ["*.pyx"],
        include_dirs = [
          "../../util/",
          "../../ginv/",
          "../../",
        ],
        libraries = [
          "ginv",
          "ginvutil",
          "gmp",
          "cgraph",
          "gvc",
        ],
        library_dirs = [
          "../ginv/",
          "../util/",
        ]
    )
]

setup(
    name="ginv",
    version = "3.0",
    description = "ginv",
    author = "Blinkov Yu.A.",
    author_email = "BlinkovUA@info.sgu.ru",
    url = "http://invo.jinr.ru",
    ext_modules=cythonize(extensions, language_level = "3"),
)

# Maintainer: Felix Barz <skycoder42.de@gmx.de>
pkgname=manga-convert
pkgver=1.1.0
pkgrel=1
pkgdesc="Download and convert mangas from single images to pdf"
arch=('i686' 'x86_64')
url="https://github.com/Skycoder42/MangaConvert"
license=('BSD')
depends=('qt5-base' 'imagemagick')
makedepends=('git' 'qt5-tools')
_pkgfqn=$pkgname-$pkgver
source=("$_pkgfqn::git+https://github.com/Skycoder42/MangaConvert.git") #tag=$pkgver")
md5sums=('SKIP')

prepare() {
  mkdir -p build
}

build() {
  cd build

  qmake -r "../$_pkgfqn/"
  make
}

package() {
  cd build

  make INSTALL_ROOT="$pkgdir" install

  install -D -m644 "../$_pkgfqn/LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

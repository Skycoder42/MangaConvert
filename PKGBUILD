# Maintainer: Felix Barz <skycoder42.de@gmx.de>
pkgname=manga-convert
pkgver=2.1.0
pkgrel=1
pkgdesc="Download and convert mangas from single images to pdf"
arch=('i686' 'x86_64')
url="https://github.com/Skycoder42/MangaConvert"
license=('BSD')
depends=('qt5-base' 'qt5-webengine')
makedepends=('git' 'qt5-tools' 'qpmx-qpmsource')
_pkgfqn=$pkgname-$pkgver
source=("$_pkgfqn::git+https://github.com/Skycoder42/MangaConvert.git#tag=$pkgver")
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

  cd "../$_pkgfqn/"
  install -D -m644 mangaconv.desktop "$pkgdir/usr/share/applications/mangaconv.desktop"
  install -D -m644 mangaconv.svg "$pkgdir/usr/share/icons/hicolor/scalable/apps/mangaconv.svg"
  install -D -m644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

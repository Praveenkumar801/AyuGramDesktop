// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ayu/ui/components/hidden_chats_bar.h"

#include "ayu/ayu_settings.h"
#include "lang_auto.h"
#include "styles/style_ayu_styles.h"
#include "styles/style_chat.h"
#include "styles/style_menu_icons.h"
#include "ui/painter.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/shadow.h"

namespace AyuUi {

namespace {

QString FormatRemaining(int seconds) {
	if (seconds <= 0) {
		return QString::fromUtf8("0:00");
	}
	const auto minutes = seconds / 60;
	const auto secs = seconds % 60;
	return QString::asprintf("%d:%02d", minutes, secs);
}

} // namespace

HiddenChatsBar::HiddenChatsBar(not_null<QWidget*> parent)
: _wrap(parent, object_ptr<Ui::RpWidget>(parent))
, _inner(_wrap.entity())
, _shadow(std::make_unique<Ui::PlainShadow>(_wrap.parentWidget()))
, _lockNow(Ui::CreateChild<Ui::LinkButton>(
	  _inner.get(),
	  tr::ayu_HiddenChatsLockNow(tr::now),
	  st::ayuHiddenChatsBarLink))
, _ticker([=] { tick(); }) {
	_wrap.hide(anim::type::instant);
	_shadow->hide();
	_inner->resize(0, st::ayuHiddenChatsBarHeight);

	_inner->paintRequest(
	) | rpl::on_next([=](QRect) {
		auto p = QPainter(_inner);
		paint(p);
	}, _inner->lifetime());
	_inner->setAttribute(Qt::WA_OpaquePaintEvent);

	_lockNow->show();
	_lockNow->setClickedCallback([] {
		AyuSettings::getInstance().setHiddenChatsRevealed(false);
	});

	_wrap.geometryValue(
	) | rpl::on_next([=](QRect geom) {
		const auto hidden = _wrap.isHidden() || !geom.height();
		if (_shadow->isHidden() != hidden) {
			_shadow->setVisible(!hidden);
		}
		_shadow->setGeometry(
			geom.x(),
			geom.y() + geom.height(),
			geom.width(),
			st::lineWidth);
	}, _inner->lifetime());

	const auto &settings = AyuSettings::getInstance();
	settings.hiddenChatsRevealedValue(
	) | rpl::on_next([=](bool revealed) {
		_wrap.toggle(revealed, anim::type::normal);
		if (revealed) {
			updateText();
			_ticker.callEach(crl::time(1000));
			_inner->update();
		} else {
			_ticker.cancel();
		}
	}, _settingsLifetime);
}

HiddenChatsBar::~HiddenChatsBar() = default;

void HiddenChatsBar::tick() {
	updateText();
	_inner->update();
}

void HiddenChatsBar::updateText() {
	const auto &settings = AyuSettings::getInstance();
	const auto autoLockSec = settings.hiddenChatsAutoLockSeconds();
	const auto revealedAt = settings.hiddenChatsRevealedAt();
	if (autoLockSec <= 0 || revealedAt <= 0) {
		_label = tr::ayu_HiddenChatsBarRevealed(tr::now);
		return;
	}
	const auto elapsed = (crl::now() - revealedAt) / crl::time(1000);
	const auto remaining = std::max<int64>(0, autoLockSec - elapsed);
	_label = tr::ayu_HiddenChatsBarRevealed(tr::now)
		+ QString::fromUtf8(" \u00B7 ")
		+ FormatRemaining(int(remaining));
}

void HiddenChatsBar::paint(QPainter &p) {
	p.fillRect(_inner->rect(), st::historyComposeAreaBg);

	const auto padding = st::ayuHiddenChatsBarPadding;
	const auto &icon = st::menuIconLock;
	const auto height = _inner->height();
	const auto iconY = (height - icon.height()) / 2;
	icon.paint(p, padding.left(), iconY, _inner->width());

	p.setFont(st::ayuHiddenChatsBarFont);
	p.setPen(st::historyComposeAreaFg);

	const auto textX = padding.left()
		+ icon.width()
		+ st::ayuHiddenChatsBarIconSkip;
	const auto available = std::max(
		0,
		_inner->width()
			- textX
			- _lockNow->width()
			- padding.right()
			- st::ayuHiddenChatsBarIconSkip);
	const auto fm = QFontMetrics(st::ayuHiddenChatsBarFont);
	const auto elided = fm.elidedText(_label, Qt::ElideRight, available);
	const auto textY = (height + fm.ascent() - fm.descent()) / 2;
	p.drawText(textX, textY, elided);
}

void HiddenChatsBar::move(int x, int y) {
	_wrap.move(x, y);
}

void HiddenChatsBar::resizeToWidth(int width) {
	_wrap.entity()->resizeToWidth(width);
	_inner->resizeToWidth(width);
	const auto padding = st::ayuHiddenChatsBarPadding;
	_lockNow->move(
		width - _lockNow->width() - padding.right(),
		(_inner->height() - _lockNow->height()) / 2);
}

int HiddenChatsBar::height() const {
	return _wrap.height();
}

rpl::producer<int> HiddenChatsBar::heightValue() const {
	return _wrap.heightValue();
}

} // namespace AyuUi

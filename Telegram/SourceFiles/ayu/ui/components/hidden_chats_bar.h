// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include "base/timer.h"
#include "ui/rp_widget.h"
#include "ui/wrap/slide_wrap.h"

namespace Ui {
class LinkButton;
class PlainShadow;
} // namespace Ui

namespace AyuUi {

class HiddenChatsBar final {
public:
	explicit HiddenChatsBar(not_null<QWidget*> parent);
	~HiddenChatsBar();

	[[nodiscard]] not_null<Ui::RpWidget*> wrap() {
		return &_wrap;
	}

	void move(int x, int y);
	void resizeToWidth(int width);
	[[nodiscard]] int height() const;
	[[nodiscard]] rpl::producer<int> heightValue() const;

	[[nodiscard]] rpl::lifetime &lifetime() {
		return _wrap.lifetime();
	}

private:
	void paint(QPainter &p);
	void tick();
	void updateText();

	Ui::SlideWrap<Ui::RpWidget> _wrap;
	not_null<Ui::RpWidget*> _inner;
	std::unique_ptr<Ui::PlainShadow> _shadow;
	Ui::LinkButton *_lockNow = nullptr;
	base::Timer _ticker;
	QString _label;
	rpl::lifetime _settingsLifetime;
};

} // namespace AyuUi

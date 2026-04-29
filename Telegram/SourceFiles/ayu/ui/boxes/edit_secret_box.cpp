// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ayu/ui/boxes/edit_secret_box.h"

#include "lang/lang_keys.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "ui/widgets/fields/password_input.h"

EditSecretBox::EditSecretBox(
	QWidget *,
	rpl::producer<QString> title,
	bool hasExistingSecret,
	Fn<void(const QString &)> saveCallback,
	Fn<void()> clearCallback)
: _title(std::move(title))
, _hasExistingSecret(hasExistingSecret)
, _saveCallback(std::move(saveCallback))
, _clearCallback(std::move(clearCallback))
, _input(this, st::defaultInputField, _title) {
}

void EditSecretBox::prepare() {
	setTitle(_title);

	auto newHeight = st::contactPadding.top()
		+ _input->height()
		+ st::boxPadding.bottom()
		+ st::contactPadding.bottom();
	setDimensions(st::boxWidth, newHeight);

	if (_hasExistingSecret && _clearCallback) {
		addLeftButton(tr::ayu_HiddenChatsClearSecret(), [=] {
			_clearCallback();
			closeBox();
		});
	}

	addButton(tr::lng_settings_save(), [=] { save(); });
	addButton(tr::lng_cancel(), [=] { closeBox(); });

	connect(_input, &Ui::PasswordInput::submitted, this, [=] { save(); });
}

void EditSecretBox::setInnerFocus() {
	_input->setFocusFast();
}

void EditSecretBox::resizeEvent(QResizeEvent *e) {
	BoxContent::resizeEvent(e);

	_input->resize(
		width()
			- st::boxPadding.left()
			- st::newGroupInfoPadding.left()
			- st::boxPadding.right(),
		_input->height());

	const auto left = st::boxPadding.left() + st::newGroupInfoPadding.left();
	_input->moveToLeft(left, st::contactPadding.top());
}

void EditSecretBox::save() {
	const auto value = _input->getLastText();
	if (value.trimmed().isEmpty()) {
		_input->setFocus();
		_input->showError();
		return;
	}
	_saveCallback(value);
	closeBox();
}

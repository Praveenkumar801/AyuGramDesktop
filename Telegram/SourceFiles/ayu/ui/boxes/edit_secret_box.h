// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include "boxes/abstract_box.h"

namespace Ui {
class PasswordInput;
} // namespace Ui

class EditSecretBox : public Ui::BoxContent {
public:
	EditSecretBox(
		QWidget *,
		rpl::producer<QString> title,
		bool hasExistingSecret,
		Fn<void(const QString &)> saveCallback,
		Fn<void()> clearCallback);

protected:
	void setInnerFocus() override;
	void prepare() override;
	void resizeEvent(QResizeEvent *e) override;

private:
	void save();

	rpl::producer<QString> _title;
	bool _hasExistingSecret = false;
	Fn<void(const QString &)> _saveCallback;
	Fn<void()> _clearCallback;

	object_ptr<Ui::PasswordInput> _input;
};

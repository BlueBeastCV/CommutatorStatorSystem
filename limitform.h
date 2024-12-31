#ifndef LIMITFORM_H
#define LIMITFORM_H

#include <QWidget>
#include "myhelper.h"

namespace Ui {
class LimitForm;
}

class LimitForm : public QWidget
{
		Q_OBJECT

	public:
		explicit LimitForm(QWidget *parent = nullptr);
		~LimitForm();

	private slots:
		void on_pushButton_clicked();

	signals:

		void hideCurrentWindow();

	private:
		Ui::LimitForm *ui;
};

#endif // LIMITFORM_H

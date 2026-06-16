// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/VS_UserWidget.h"

void UVS_UserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

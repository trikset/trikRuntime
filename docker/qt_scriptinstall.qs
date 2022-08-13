function Controller() {
  installer.autoRejectMessageBoxes();
  installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
  installer.setMessageBoxAutomaticAnswer("stopProcessesForUpdates", QMessageBox.Ignore);
  installer.installationFinished.connect(function() {
    gui.clickButton(buttons.NextButton);
  });
  installer.setMessageBoxAutomaticAnswer("cancelInstallation", QMessageBox.Yes);
}

Controller.prototype.WelcomePageCallback = function() {
  console.log("Welcome page");
  gui.clickButton(buttons.NextButton, 10000);
}

Controller.prototype.CredentialsPageCallback = function() {
    console.log("Credentials page");
    var widget = gui.currentPageWidget();
    widget.loginWidget.EmailLineEdit.setText("");
    widget.loginWidget.PasswordLineEdit.setText("");
    gui.clickButton(buttons.NextButton, 500);
}

Controller.prototype.ObligationsPageCallback = function()
{
  console.log("Obligations page");
  var page = gui.pageWidgetByObjectName("ObligationsPage");
  page.obligationsAgreement.setChecked(true);
  page.completeChanged();
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function() {
  console.log("Introduction page");
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.DynamicTelemetryPluginFormCallback = function() {
  console.log("Telemetry page");
  var page = gui.pageWidgetByObjectName("DynamicTelemetryPluginForm");
  page.statisticGroupBox.disableStatisticRadioButton.setChecked(true);
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
  console.log("Directory page");
  var widget = gui.currentPageWidget()
  if (widget != null) {
    var targetDir = installer.environmentVariable("TRIK_QT_INSTALL_DIR");
    if (targetDir == "") {
      targetDir = installer.value("HomeDir") + "/Qt";
    }
    widget.TargetDirectoryLineEdit.setText(targetDir);
  }
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function() {
  console.log("License page");
  var x = gui.currentPageWidget().AcceptLicenseRadioButton || gui.currentPageWidget().AcceptLicenseCheckBox;
  x.setChecked(true);
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
  console.log("Components page");
  var widget = gui.currentPageWidget();
  var targetPlatform = {"darwin":"clang_64", "winnt":"undefined", "linux":"gcc_64"}[systemInfo.kernelType];
  widget.deselectAll();
  var qtVersionId = "qt.qt5.5127."
  widget.selectComponent(qtVersionId + targetPlatform);
  widget.selectComponent(qtVersionId + "qtscript");
  widget.selectComponent("qt.tools.ifw.32");

  installer.calculateComponentsToInstall();

  var examplesId = qtVersionId + "examples";
  var docId = qtVersionId + "doc";

  var components = installer.components();
  for (var i = 0 ; i < components.length ;i++) {
    var c = components[i];
    var n = c.name;
    if (n.startsWith(examplesId) || n.startsWith(docId)) {
      widget.deselectComponent(n);
      print("Deselecting:" +  n);
    }
  }
  widget.deselectComponent(examplesId);
  widget.deselectComponent(docId);
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function() {
  console.log("Ready to install");
  gui.clickButton(buttons.CommitButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
    console.log("Start menu page");
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.PerformInstallationPageCallback = function() {
  console.log("Perform installation page");
  gui.clickButton(buttons.CommitButton);
}

Controller.prototype.FinishedPageCallback = function() {
  console.log("Finished page");
  var page = gui.currentPageWidget();
  var checkBoxForm = page.LaunchQtCreatorCheckBoxForm;
  if (checkBoxForm && checkBoxForm.launchQtCreatorCheckBox) {
    checkBoxForm.launchQtCreatorCheckBox.setChecked(false);
  }
  if (page.RunItCheckBox) {
    page.RunItCheckBox.setChecked(false);
  }
  gui.clickButton(buttons.FinishButton);
}

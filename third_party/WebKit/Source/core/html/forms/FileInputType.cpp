/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "core/html/forms/FileInputType.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/Event.h"
#include "core/fileapi/File.h"
#include "core/fileapi/FileList.h"
#include "core/frame/UseCounter.h"
#include "core/html/FormDataList.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/forms/FormController.h"
#include "core/layout/LayoutFileUploadControl.h"
#include "core/page/ChromeClient.h"
#include "core/page/DragData.h"
#include "platform/FileMetadata.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/UserGestureIndicator.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

using blink::WebLocalizedString;
using namespace HTMLNames;

inline FileInputType::FileInputType(HTMLInputElement& element)
    : BaseClickableWithKeyInputType(element)
    , m_fileList(FileList::create())
{
}

PassRefPtrWillBeRawPtr<InputType> FileInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new FileInputType(element));
}

DEFINE_TRACE(FileInputType)
{
    visitor->trace(m_fileList);
    BaseClickableWithKeyInputType::trace(visitor);
}

Vector<FileChooserFileInfo> FileInputType::filesFromFormControlState(const FormControlState& state)
{
    Vector<FileChooserFileInfo> files;
    for (size_t i = 0; i < state.valueSize(); i += 2) {
        if (!state[i + 1].isEmpty())
            files.append(FileChooserFileInfo(state[i], state[i + 1]));
        else
            files.append(FileChooserFileInfo(state[i]));
    }
    return files;
}

const AtomicString& FileInputType::formControlType() const
{
    return InputTypeNames::file;
}

FormControlState FileInputType::saveFormControlState() const
{
    if (m_fileList->isEmpty())
        return FormControlState();
    FormControlState state;
    unsigned numFiles = m_fileList->length();
    for (unsigned i = 0; i < numFiles; ++i) {
        if (m_fileList->item(i)->hasBackingFile()) {
            state.append(m_fileList->item(i)->path());
            state.append(m_fileList->item(i)->name());
        }
        // FIXME: handle Blob-backed File instances, see http://crbug.com/394948
    }
    return state;
}

void FileInputType::restoreFormControlState(const FormControlState& state)
{
    if (state.valueSize() % 2)
        return;
    filesChosen(filesFromFormControlState(state));
}

bool FileInputType::appendFormData(FormDataList& encoding, bool multipart) const
{
    FileList* fileList = element().files();
    unsigned numFiles = fileList->length();
    if (!multipart) {
        // Send only the basenames.
        // 4.10.16.4 and 4.10.16.6 sections in HTML5.

        // Unlike the multipart case, we have no special handling for the empty
        // fileList because Netscape doesn't support for non-multipart
        // submission of file inputs, and Firefox doesn't add "name=" query
        // parameter.
        for (unsigned i = 0; i < numFiles; ++i)
            encoding.appendData(element().name(), fileList->item(i)->name());
        return true;
    }

    // If no filename at all is entered, return successful but empty.
    // Null would be more logical, but Netscape posts an empty file. Argh.
    if (!numFiles) {
        encoding.appendBlob(element().name(), File::create(""));
        return true;
    }

    for (unsigned i = 0; i < numFiles; ++i)
        encoding.appendBlob(element().name(), fileList->item(i));
    return true;
}

bool FileInputType::valueMissing(const String& value) const
{
    return element().isRequired() && value.isEmpty();
}

String FileInputType::valueMissingText() const
{
    return locale().queryString(element().multiple() ? WebLocalizedString::ValidationValueMissingForMultipleFile : WebLocalizedString::ValidationValueMissingForFile);
}

void FileInputType::handleDOMActivateEvent(Event* event)
{
    if (element().isDisabledFormControl())
        return;

    if (!UserGestureIndicator::processingUserGesture())
        return;

    if (ChromeClient* chromeClient = this->chromeClient()) {
        FileChooserSettings settings;
        HTMLInputElement& input = element();
        settings.allowsDirectoryUpload = input.fastHasAttribute(webkitdirectoryAttr);
        settings.allowsMultipleFiles = settings.allowsDirectoryUpload || input.fastHasAttribute(multipleAttr);
        settings.acceptMIMETypes = input.acceptMIMETypes();
        settings.acceptFileExtensions = input.acceptFileExtensions();
        settings.selectedFiles = m_fileList->pathsForUserVisibleFiles();
        settings.useMediaCapture = RuntimeEnabledFeatures::mediaCaptureEnabled() && input.fastHasAttribute(captureAttr);
        chromeClient->openFileChooser(input.document().frame(), newFileChooser(settings));
    }
    event->setDefaultHandled();
}

LayoutObject* FileInputType::createLayoutObject(const ComputedStyle&) const
{
    return new LayoutFileUploadControl(&element());
}

bool FileInputType::canSetStringValue() const
{
    return false;
}

FileList* FileInputType::files()
{
    return m_fileList.get();
}

bool FileInputType::canSetValue(const String& value)
{
    // For security reasons, we don't allow setting the filename, but we do allow clearing it.
    // The HTML5 spec (as of the 10/24/08 working draft) says that the value attribute isn't
    // applicable to the file upload control at all, but for now we are keeping this behavior
    // to avoid breaking existing websites that may be relying on this.
    return value.isEmpty();
}

bool FileInputType::getTypeSpecificValue(String& value)
{
    if (m_fileList->isEmpty()) {
        value = String();
        return true;
    }

    // HTML5 tells us that we're supposed to use this goofy value for
    // file input controls. Historically, browsers revealed the real
    // file path, but that's a privacy problem. Code on the web
    // decided to try to parse the value by looking for backslashes
    // (because that's what Windows file paths use). To be compatible
    // with that code, we make up a fake path for the file.
    value = "C:\\fakepath\\" + m_fileList->item(0)->name();
    return true;
}

void FileInputType::setValue(const String&, bool valueChanged, TextFieldEventBehavior)
{
    if (!valueChanged)
        return;

    m_fileList->clear();
    element().setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::ControlValue));
    element().setNeedsValidityCheck();
}

FileList* FileInputType::createFileList(const Vector<FileChooserFileInfo>& files, bool hasWebkitDirectoryAttr)
{
    FileList* fileList(FileList::create());
    size_t size = files.size();

    // If a directory is being selected, the UI allows a directory to be chosen
    // and the paths provided here share a root directory somewhere up the tree;
    // we want to store only the relative paths from that point.
    if (size && hasWebkitDirectoryAttr) {
        // Find the common root path.
        String rootPath = directoryName(files[0].path);
        for (size_t i = 1; i < size; ++i) {
            while (!files[i].path.startsWith(rootPath))
                rootPath = directoryName(rootPath);
        }
        rootPath = directoryName(rootPath);
        ASSERT(rootPath.length());
        int rootLength = rootPath.length();
        if (rootPath[rootLength - 1] != '\\' && rootPath[rootLength - 1] != '/')
            rootLength += 1;
        for (size_t i = 0; i < size; ++i) {
            // Normalize backslashes to slashes before exposing the relative path to script.
            String relativePath = files[i].path.substring(rootLength).replace('\\', '/');
            fileList->append(File::createWithRelativePath(files[i].path, relativePath));
        }
        return fileList;
    }

    for (size_t i = 0; i < size; ++i) {
        if (files[i].fileSystemURL.isEmpty()) {
            fileList->append(File::createForUserProvidedFile(files[i].path, files[i].displayName));
        } else {
            fileList->append(File::createForFileSystemFile(files[i].fileSystemURL, files[i].metadata, File::IsUserVisible));
        }
    }
    return fileList;
}

void FileInputType::countUsage()
{
    // It is required by isPrivilegedContext() but isn't
    // actually used. This could be used later if a warning is shown in the
    // developer console.
    String insecureOriginMsg;
    Document* document = &element().document();
    if (document->isPrivilegedContext(insecureOriginMsg))
        UseCounter::count(*document, UseCounter::InputTypeFileInsecureOrigin);
    else
        UseCounter::count(*document, UseCounter::InputTypeFileSecureOrigin);
}

void FileInputType::createShadowSubtree()
{
    ASSERT(element().shadow());
    RefPtrWillBeRawPtr<HTMLInputElement> button = HTMLInputElement::create(element().document(), 0, false);
    button->setType(InputTypeNames::button);
    button->setAttribute(valueAttr, AtomicString(locale().queryString(element().multiple() ? WebLocalizedString::FileButtonChooseMultipleFilesLabel : WebLocalizedString::FileButtonChooseFileLabel)));
    button->setShadowPseudoId(AtomicString("-webkit-file-upload-button", AtomicString::ConstructFromLiteral));
    element().userAgentShadowRoot()->appendChild(button.release());
}

void FileInputType::disabledAttributeChanged()
{
    ASSERT(element().shadow());
    if (Element* button = toElement(element().userAgentShadowRoot()->firstChild()))
        button->setBooleanAttribute(disabledAttr, element().isDisabledFormControl());
}

void FileInputType::multipleAttributeChanged()
{
    ASSERT(element().shadow());
    if (Element* button = toElement(element().userAgentShadowRoot()->firstChild()))
        button->setAttribute(valueAttr, AtomicString(locale().queryString(element().multiple() ? WebLocalizedString::FileButtonChooseMultipleFilesLabel : WebLocalizedString::FileButtonChooseFileLabel)));
}

void FileInputType::setFiles(FileList* files)
{
    if (!files)
        return;

    RefPtrWillBeRawPtr<HTMLInputElement> input(element());

    bool filesChanged = false;
    if (files->length() != m_fileList->length()) {
        filesChanged = true;
    } else {
        for (unsigned i = 0; i < files->length(); ++i) {
            if (!files->item(i)->hasSameSource(*m_fileList->item(i))) {
                filesChanged = true;
                break;
            }
        }
    }

    m_fileList = files;

    input->notifyFormStateChanged();
    input->setNeedsValidityCheck();

    if (input->layoutObject())
        input->layoutObject()->setShouldDoFullPaintInvalidation();

    if (filesChanged) {
        // This call may cause destruction of this instance.
        // input instance is safe since it is ref-counted.
        input->dispatchChangeEvent();
    }
    input->setChangedSinceLastFormControlChangeEvent(false);
}

void FileInputType::filesChosen(const Vector<FileChooserFileInfo>& files)
{
    setFiles(createFileList(files, element().fastHasAttribute(webkitdirectoryAttr)));
}

void FileInputType::receiveDropForDirectoryUpload(const Vector<String>& paths)
{
    if (ChromeClient* chromeClient = this->chromeClient()) {
        FileChooserSettings settings;
        HTMLInputElement& input = element();
        settings.allowsDirectoryUpload = true;
        settings.allowsMultipleFiles = true;
        settings.selectedFiles.append(paths[0]);
        settings.acceptMIMETypes = input.acceptMIMETypes();
        settings.acceptFileExtensions = input.acceptFileExtensions();
        chromeClient->enumerateChosenDirectory(newFileChooser(settings));
    }
}

bool FileInputType::receiveDroppedFiles(const DragData* dragData)
{
    Vector<String> paths;
    dragData->asFilePaths(paths);
    if (paths.isEmpty())
        return false;

    HTMLInputElement& input = element();
    if (input.fastHasAttribute(webkitdirectoryAttr)) {
        receiveDropForDirectoryUpload(paths);
        return true;
    }

    m_droppedFileSystemId = dragData->droppedFileSystemId();

    Vector<FileChooserFileInfo> files;
    for (unsigned i = 0; i < paths.size(); ++i)
        files.append(FileChooserFileInfo(paths[i]));

    if (input.fastHasAttribute(multipleAttr)) {
        filesChosen(files);
    } else {
        Vector<FileChooserFileInfo> firstFileOnly;
        firstFileOnly.append(files[0]);
        filesChosen(firstFileOnly);
    }
    return true;
}

String FileInputType::droppedFileSystemId()
{
    return m_droppedFileSystemId;
}

String FileInputType::defaultToolTip() const
{
    FileList* fileList = m_fileList.get();
    unsigned listSize = fileList->length();
    if (!listSize) {
        return locale().queryString(WebLocalizedString::FileButtonNoFileSelectedLabel);
    }

    StringBuilder names;
    for (size_t i = 0; i < listSize; ++i) {
        names.append(fileList->item(i)->name());
        if (i != listSize - 1)
            names.append('\n');
    }
    return names.toString();
}

} // namespace blink

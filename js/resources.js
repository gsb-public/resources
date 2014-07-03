(function ($) {

  /**
   * Attaches resources browser to every textarea in the form that has a resources-browser class.
   */
  Drupal.behaviors.attachResourcesBrowser = {
    attach: function (context, settings) {
      $('.resources-browser', context).once('resources-browser', function () {
        var params = Drupal.settings.resources.triggers[this.id];
        Drupal.resources.browserAttach(context, params);
      });
    }
  };


  /**
   * Attaches outline behavior for regions associated with contextual links.
   */
  Drupal.behaviors.resourcesOperations = {
    attach: function (context) {
      var $wrapper = $('#resources-wrapper');
      $wrapper.find('.resource-operations-trigger').once('resources-operations-trigger', function () {
        $(this).hover(
          function () {
            $(this).closest('.resource-item').find('ul.resource-operations').stop(true, true).toggle();
            return false;
          },
          function () {
            $(this).closest('.resource-item').find('ul.resource-operations').stop(true, true).toggle();
            return false;
          }
        );

        $wrapper.find('ul.resource-operations').hover(
          function () {
            $(this).stop(true, true).show();
          },
          function () {
            $(this).stop(true, true).hide();
          }
        )
      });
    }
  };

  Drupal.behaviors.resources = {
    attach: function (context) {
      $('a.resource-insert').once('resource-insert').click(function (event) {
        token = $(this).data('resource-info');

        viewMode = 'default';
        $topElement = $(this).closest('.resource-item').find('.resource-view-mode-selection select');
        if ($topElement.length) {
          viewMode = $topElement.val();
        }

        token += viewMode + ']]';
        Drupal.resources.insert.insertIntoActiveEditor(token);
        event.preventDefault();
      });

      if (typeof(insertTextarea) == 'undefined') {
        // @todo: Find first text area that has resources enabled instead.
        insertTextarea = $('#edit-body textarea.text-full').get(0) || false;
      }

      // Keep track of the last active textarea (if not using WYSIWYG).
      $('.node-form textarea:not([name$="[data][title]"])', context).focus(insertSetActive).blur(insertRemoveActive);

      function insertSetActive() {
        insertTextarea = this;
        this.insertHasFocus = true;
      }

      function insertRemoveActive() {
        if (insertTextarea == this) {
          var thisTextarea = this;
          setTimeout(function() {
            thisTextarea.insertHasFocus = false;
          }, 1000);
        }
      }

    }
  };

  Drupal.resources = Drupal.resources || {};

  Drupal.resources.browserAttach = function (context, params) {
    var browser_button = $('<a href="#" class="resources-browser-trigger">Insert resource</a>').click(function(event) {
      event.preventDefault();
      Drupal.resources.browserShow(this, params.field);
    });
    var browser_toolbar = $('<div class="resources-browser-toolbar"></div>').append(browser_button);
    var browser_placeholder = $('<div class="resources-browser-placeholder"></div>');
    var browser_wrapper = $("<div class='resources-toolbar-wrapper'></div>");

    browser_wrapper.append(browser_toolbar, browser_placeholder);
    $('#'+ params.field, context).before(browser_wrapper);
    //$('#'+ params.field, context).before(browser_button, browser_placeholder);
  };

  Drupal.resources.browserShow = function(link, field) {
    var resources_browser = $('.resources-browser-wrapper').clone(true);
    $('.resources-browser-wrapper').remove();
    $('#' + field).closest('.form-item').find('.resources-browser-placeholder').html(resources_browser);

    if ($(link).hasClass('active')) {
      $('.resources-browser-trigger').removeClass('active');
      resources_browser.hide(400);
    }
    else {
      $('.resources-browser-trigger').removeClass('active');
      $(link).addClass('active');
      resources_browser.show(400);
    }
  };

  // General Insert API functions.
  Drupal.resources.insert = {
    /**
     * Insert content into the current (or last active) editor on the page. This
     * should work with most WYSIWYGs as well as plain textareas.
     *
     * @param content
     */
    insertIntoActiveEditor: function(content) {
      // Always work in normal text areas that currently have focus.
      if (insertTextarea && insertTextarea.insertHasFocus) {
        Drupal.resources.insert.insertAtCursor(insertTextarea, content);
      }
      // Direct tinyMCE support.
      else if (typeof(tinyMCE) != 'undefined' && tinyMCE.activeEditor) {
        Drupal.resources.insert.activateTabPane(document.getElementById(tinyMCE.activeEditor.editorId));
        tinyMCE.activeEditor.execCommand('mceInsertContent', false, content);
      }
      // WYSIWYG support, should work in all editors if available.
      else if (Drupal.wysiwyg && Drupal.wysiwyg.activeId) {
        Drupal.resources.insert.activateTabPane(document.getElementById(Drupal.wysiwyg.activeId));
        Drupal.wysiwyg.instances[Drupal.wysiwyg.activeId].insert(content)
      }
      // FCKeditor module support.
      else if (typeof(FCKeditorAPI) != 'undefined' && typeof(fckActiveId) != 'undefined') {
        Drupal.resources.insert.activateTabPane(document.getElementById(fckActiveId));
        FCKeditorAPI.Instances[fckActiveId].InsertHtml(content);
      }
      // Direct FCKeditor support (only body field supported).
      else if (typeof(FCKeditorAPI) != 'undefined') {
        // Try inserting into the body.
        if (FCKeditorAPI.Instances[insertTextarea.id]) {
          Drupal.resources.insert.activateTabPane(insertTextarea);
          FCKeditorAPI.Instances[insertTextarea.id].InsertHtml(content);
        }
        // Try inserting into the first instance we find (may occur with very
        // old versions of FCKeditor).
        else {
          for (var n in FCKeditorAPI.Instances) {
            Drupal.resources.insert.activateTabPane(document.getElementById(n));
            FCKeditorAPI.Instances[n].InsertHtml(content);
            break;
          }
        }
      }
      // CKeditor module support.
      else if (typeof(CKEDITOR) != 'undefined' && typeof(Drupal.ckeditorActiveId) != 'undefined') {
        Drupal.resources.insert.activateTabPane(document.getElementById(Drupal.ckeditorActiveId));
        CKEDITOR.instances[Drupal.ckeditorActiveId].insertHtml(content);
      }
      // Direct CKeditor support (only body field supported).
      else if (typeof(CKEDITOR) != 'undefined' && CKEDITOR.instances[insertTextarea.id]) {
        Drupal.resources.insert.activateTabPane(insertTextarea);
        CKEDITOR.instances[insertTextarea.id].insertHtml(content);
      }
      else if (insertTextarea) {
        Drupal.resources.insert.activateTabPane(insertTextarea);
        Drupal.resources.insert.insertAtCursor(insertTextarea, content);
      }

      return false;
    },

    /**
     * Check for vertical tabs and activate the pane containing the editor.
     *
     * @param editor
     *   The DOM object of the editor that will be checked.
     */
    activateTabPane: function(editor) {
      var $pane = $(editor).parents('.vertical-tabs-pane:first');
      var $panes = $pane.parent('.vertical-tabs-panes');
      var $tabs = $panes.parents('.vertical-tabs:first').find('ul.vertical-tabs-list:first li a');
      if ($pane.size() && $pane.is(':hidden') && $panes.size() && $tabs.size()) {
        var index = $panes.children().index($pane);
        $tabs.eq(index).click();
      }
    },

    /**
     * Insert content into a textarea at the current cursor position.
     *
     * @param editor
     *   The DOM object of the textarea that will receive the text.
     * @param content
     *   The string to be inserted.
     */
    insertAtCursor: function(editor, content) {
      // Record the current scroll position.
      var scroll = editor.scrollTop;

      // IE support.
      if (document.selection) {
        editor.focus();
        sel = document.selection.createRange();
        sel.text = content;
      }

      // Mozilla/Firefox/Netscape 7+ support.
      else if (editor.selectionStart || editor.selectionStart == '0') {
        var startPos = editor.selectionStart;
        var endPos = editor.selectionEnd;
        editor.value = editor.value.substring(0, startPos) + content + editor.value.substring(endPos, editor.value.length);
      }

      // Fallback, just add to the end of the content.
      else {
        editor.value += content;
      }

      // Ensure the textarea does not unexpectedly scroll.
      editor.scrollTop = scroll;
    }
  };
})(jQuery);
(function ($) {

  /**
   * Provides an AJAX command to add the new resource to the page.
   */
  Drupal.ajax.prototype.commands.resources_add_resource = function (ajax, response, status) {
    resource = $(response.resource);

    $('#resources-wrapper').append(resource);
    Drupal.attachBehaviors(resource, Drupal.settings);

    $elResourcesInput = $('#edit-resources-ids');
    if ($elResourcesInput.length > 0) {
      ids = $elResourcesInput.val();
      if (ids == '') {
        arrIDS = [];
      }
      else {
        arrIDS = ids.split(';');
      }
      if (arrIDS.indexOf(response.fid) == -1) {
        arrIDS.push(response.fid);
      }
      ids = arrIDS.join(';');
      $elResourcesInput.val(ids);
    }
  };

  /**
   * Provides an AJAX command to edit a resource on the page\.
   */
  Drupal.ajax.prototype.commands.resources_edit_resource = function (ajax, response, status) {
    resource = $(response.resource);
    $('#resource-' + response.fid).replaceWith(resource);
    Drupal.attachBehaviors(resource, Drupal.settings);
  };

  /**
   * Provides an AJAX command to delete a resource on the page.
   */
  Drupal.ajax.prototype.commands.resources_delete_resource = function (ajax, response, status) {
    $('#resource-' + response.fid).remove();
  };

}(jQuery));

<?php
  if ($form['title']['#value'] == '') {
    $form['title']['#value'] = 'Test' . rand(0,10000);
  }
?>

<div>
  <?php print drupal_render($form['title']); ?>
</div>
<div>
  <?php print drupal_render($form['field_requested_cores']); ?>
</div>
<div>
  <?php print drupal_render($form['field_requested_tasks']); ?>
</div>
<div>
  <?php print drupal_render($form['field_csize']); ?>
</div>
<div>
  <?php print drupal_render($form['field_file']); ?>
</div>
<div>
  <?php print drupal_render($form['field_analysis_setup']); ?>
</div>
<div>
  <?php print drupal_render($form['actions']); ?>
</div>
<?php
//dsm($form);


<?php

/**
 * @file
 * Isotropic library file.
 *
 */

function form_isotropic_job_config($form, $form_state, $nid) {

  // Parse uploaded file
  $node = node_load($nid);
  $file = file_load($node->field_file['und'][0]['fid']);
  $filename = backdrop_basename($file->uri);
  $path = backdrop_realpath($file->uri);
  $contents = file_get_contents($path);
  $lines = explode("\n", $contents);
  $parsed = array();
  //dpm(sizeof($lines));

  foreach($lines as $word) {
    $t = explode("\t", $word);
    $s = sizeof($t) - 1;
    $lc = 0;
    switch (sizeof($t)) {
      case 1:
        $parsed[] = $t[$s];
        break;
      case 2:
        $parsed[][] = $t[$s];
        break;
      case 3:
        $parsed[][][] = $t[$s];
        break;
      case 4:
        $parsed[][][][] = $t[$s];
        break;
      case 5:
        $parsed[][][][][] = $t[$s];
        break;
      case 6:
        $parsed[][][][][][] = $t[$s];
        break;
      case 7:
        $parsed[][][][][][][] = $t[$s];
        break;
      case 8:
        $parsed[][][][][][][][] = $t[$s];
        break;
      case 9:
        $parsed[][][][][][][][][] = $t[$s];
        break;
      case 10:
        $parsed[][][][][][][][][][] = $t[$s];
        break;
      case 11:
        $parsed[][][][][][][][][][][] = $t[$s];
        break;
      case 12:
        $parsed[][][][][][][][][][][][] = $t[$s];
        break;
      default:
        $parsed[] = $t[$s];
        break;
    }
    //echo $word;
    $lc++;
  }
  //dpm(sizeof($parsed));
  $sa = array();

  for ($a = 0; $a < sizeof($parsed); $a++) {
    for ($b = 0; $b < sizeof($parsed[$a]); $b++) {
      for ($c = 0; $c < sizeof($parsed[$a][$b]); $c++) {
        if (strstr($parsed[$a][$b][$c], "begin 'AnalysisSetup")) {
          $sa[$a]['start'] = $a;
          $reld = $a;
        }
        if (strstr($parsed[$a][$b][$c], "end 'AnalysisSetup")) {
          $sa[$reld]['end'] = $a;
        }
        if (strstr($parsed[$a][$b][$c], "begin 'Optimetrics")) {
          $sa[$a]['opstart'] = $a;
          $reld = $a;
        }
        if (strstr($parsed[$a][$b][$c], "end 'Optimetrics")) {
          $sa[$reld]['opend'] = $a;
        }
      }
    }
  }
  //dsm ($sa);

  $params = array();
  $designs = array();
  $sweep = array();
  $optim = array();

  foreach ($sa as $lineno => $linelist) {

    //Design Iteration
    $a = '';
    for ($a = $linelist['start']; $a <= $linelist['end']; $a++) {
      if (strstr($parsed[$a][0][0][0][0], "begin")) {
        $params[$a][$a]['design_name'] = $parsed[$a][0][0][0][0];
        $rel = $a;
      }
      if (strstr($parsed[$a][0][0][0][0][0], "IsEnabled")) {
        $params[$rel][$a]['enabled'] = $parsed[$a][0][0][0][0][0];
      }
      if (strstr($parsed[$a][0][0][0][0][0][0][0], "IsEnabled")) {
        $params[$rel][$a]['enabledsweep'] = $parsed[$a][0][0][0][0][0][0][0];
      }
    }

    //Optimetrics Iteration
    $a = '';
    for ($a = $linelist['opstart']; $a <= $linelist['opend']; $a++) {
      if (strstr($parsed[$a][0][0][0][0][0], "IsEnabled")) {
        $params[$a][$a]['enabledopt'] = 'Optimetrics';
      }
    }
  }

  //dsm($params);

  // Create parameter selections
  foreach ($params as $parentline => $payload) {
    foreach ($payload as $paramline => $linepayload) {
      if (isset($linepayload['design_name'])) {
        $design_name = preg_replace('/\$begin\s/', '', str_replace("'", "", $linepayload['design_name']));
        $designs[$paramline] = $design_name;
        $assoc_enabled = $design_name;
      }
      if (isset($linepayload['enabled'])) {
        $enabled[$paramline] = $assoc_enabled;
      }
      if (isset($linepayload['enabledsweep'])) {
        $sweep[$paramline] = $assoc_enabled;
      }
      if (isset($linepayload['enabledopt'])) {
        $optim[$paramline] = $linepayload['enabledopt'];
      }
    }
  }

  // Removed last element of optim; picking up an extra according to specs.
  array_pop($optim);

  $form['designs'] = array(
    '#title' => t('Designs Available'),
    '#type' => 'checkboxes',
    '#description' => t('Select the designs to enable.'),
    '#options' => $enabled,
  );

  $form['sweeps'] = array(
    '#title' => t('Sweeps By Design'),
    '#type' => 'checkboxes',
    '#description' => t('Select the sweeps associated with each design to enable.'),
    '#options' => $sweep,
  );

  $form['optimetrics'] = array(
    '#title' => t('Optimetrics'),
    '#type' => 'checkboxes',
    '#description' => t('Select wheter or not to enable optimetrics.'),
    '#options' => $optim,
  );

  $form['node_info'] = array(
    '#type' => 'hidden',
    '#value' => $nid,
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => 'Submit Job',
    '#submit' => array('form_jobconfig_submit',),
    '#validate' => array('form_jobconfig_validate',),
  );

  return $form;
}

function form_jobconfig_validate($form, &$form_state) {

}

function form_jobconfig_submit($form, &$form_state) {

  // Save uploaded file

  $line_set = array();
  // designs
  foreach ($form['designs']['#options'] as $lineno => $value) {
    if ($form['designs'][$lineno]['#checked']) {
      $line_set[$lineno] = true;
    }
    else {
      $line_set[$lineno] = false;
    }
  }

  // sweeps
  foreach ($form['sweeps']['#options'] as $lineno => $value) {
    if ($form['sweeps'][$lineno]['#checked']) {
      $line_set[$lineno] = true;
    }
    else {
      $line_set[$lineno] = false;
    }
  }

  // optim
  foreach ($form['optimetrics']['#options'] as $lineno => $value) {
    if ($form['optimetrics'][$lineno]['#checked']) {
      $line_set[$lineno] = true;
    }
    else {
      $line_set[$lineno] = false;
    }
  }

  $node = node_load($form['node_info']['#value']);
  $file = file_load($node->field_file['und'][0]['fid']);
  $filename = backdrop_basename($file->uri);
  $path = backdrop_realpath($file->uri);
  $content = file($path);

  foreach($content as $line_number => &$line_content) {
    foreach ($line_set as $line_key => $line_val) {
      if ($line_number == $line_key) {
        $line_content = $line_val ? str_replace('false', 'true', $line_content) : str_replace('true', 'false', $line_content);
      }
    }
  }

  $all_content = implode("", $content);
  file_save_data($all_content, $file->uri, FILE_EXISTS_REPLACE);

  // Execute job
  $form_state['rebuild'] = TRUE;

  $job_submitted = false;

  $job_submitted = isotropic_execute_job(
    $filename,
    $node->title,
    //$node->field_analysis_setup['und'][0]['value'],
    NULL,
    $node->field_csize['und'][0]['value'],
    $node->field_requested_cores['und'][0]['value'],
    $node->field_requested_tasks['und'][0]['value']);

  if ($job_submitted) {
    backdrop_set_message(t('Job Submit Successful.'));
    $node->field_job_status[LANGUAGE_NONE]['0']['value'] = 1; //Running
    //dsm($node);
  }
  else {
      backdrop_set_message(t('Job Submission Error.'), 'error');
      $node->field_job_status[LANGUAGE_NONE]['0']['value'] = 0; //Failed
  }
  field_attach_update('node', $node); //Save field updates (e.g. job status)

}







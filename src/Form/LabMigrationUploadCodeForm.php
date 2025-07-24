<?php

namespace Drupal\lab_migration\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Url;
use Drupal\Core\Link;
use Drupal\Core\Database\Database;

/**
 * Provides the Upload Code Form.
 */
class LabMigrationUploadCodeForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'lab_migration_upload_code_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = \Drupal::config('lab_migration.settings');

    $proposal_data = \Drupal::service('lab_migration_global')->lab_migration_get_proposal();
    if (!$proposal_data) {
      (new RedirectResponse(Url::fromRoute('lab_migration.proposal')->toString()))->send();
      return [];
    }

    $form['#attributes']['enctype'] = 'multipart/form-data';

    $form['lab_title'] = [
      '#type' => 'item',
      '#title' => $this->t('Title of the Lab'),
      '#markup' => $proposal_data->lab_title,
    ];

    $form['name'] = [
      '#type' => 'item',
      '#title' => $this->t('Proposer Name'),
      '#markup' => $proposal_data->name_title . ' ' . $proposal_data->name,
    ];

    $query = \Drupal::database()->select('lab_migration_experiment', 'e')
      ->fields('e')
      ->condition('proposal_id', $proposal_data->id)
      ->orderBy('id', 'ASC');
    $experiment_q = $query->execute();

    $experiment_rows = [];
    foreach ($experiment_q as $experiment_data) {
      $experiment_rows[$experiment_data->id] = $experiment_data->number . '. ' . $experiment_data->title;
    }

    $form['experiment'] = [
      '#type' => 'select',
      '#title' => $this->t('Title of the Experiment'),
      '#options' => $experiment_rows,
      '#required' => TRUE,
    ];

    $form['code_number'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Code No'),
      '#required' => TRUE,
    ];

    $form['code_caption'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Caption'),
      '#required' => TRUE,
    ];

    $form['os_used'] = [
      '#type' => 'select',
      '#title' => $this->t('Operating System used'),
      '#options' => [
        'Linux' => 'Linux',
        'Windows' => 'Windows',
        'Mac' => 'Mac',
      ],
      '#required' => TRUE,
    ];

    $form['esim_version'] = [
      '#type' => 'select',
      '#title' => $this->t('Esim version used'),
      '#options' => \Drupal::service('lab_migration_global')->_lm_list_of_esim_version(),
      '#required' => TRUE,
    ];

    $form['code_warning'] = [
      '#type' => 'item',
      '#title' => $this->t('Upload all the eSim project files in .zip format'),
      '#prefix' => '<div style="color:red">',
      '#suffix' => '</div>',
    ];

    $form['sourcefile1'] = [
      '#type' => 'file',
      '#title' => $this->t('Upload main or source file'),
      '#description' => $this->t('Allowed: ') . ($config->get('lab_migration_source_extensions') ?? ''),
    ];

    $form['samplemarkup'] = [
      '#type' => 'markup',
      '#markup' => "<a href='http://esim.fossee.in/resource/book/analysis_of_BJT_amplr.pdf' target='_blank'>View Sample PDF</a>",
    ];

    $form['chppdf'] = [
      '#type' => 'file',
      '#title' => $this->t('Upload PDF File'),
      '#description' => $this->t('Allowed: ') . ($config->get('lab_migration_pdf_extensions') ?? ''),
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Submit'),
    ];

    $form['cancel_link'] = [
      '#type' => 'markup',
      '#markup' => Link::fromTextAndUrl($this->t('Cancel'), Url::fromRoute('lab_migration.list_experiments'))->toString(),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    if (!$this->lab_migration_check_code_number($form_state->getValue('code_number'))) {
      $form_state->setErrorByName('code_number', $this->t('Invalid Code Number.'));
    }
    if (!$this->lab_migration_check_name($form_state->getValue('code_caption'))) {
      $form_state->setErrorByName('code_caption', $this->t('Invalid Caption.'));
    }

    if (!isset($_FILES['files']['name']['sourcefile1']) || empty($_FILES['files']['name']['sourcefile1'])) {
      $form_state->setErrorByName('sourcefile1', $this->t('Please upload the source file.'));
    }

    if (!isset($_FILES['files']['name']['chppdf']) || empty($_FILES['files']['name']['chppdf'])) {
      $form_state->setErrorByName('chppdf', $this->t('Please upload the PDF file.'));
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $proposal_data = \Drupal::service('lab_migration_global')->lab_migration_get_proposal();
    $root_path = \Drupal::service('lab_migration_global')->lab_migration_path();

    $experiment_id = (int) $form_state->getValue('experiment');
    $experiment = \Drupal::database()->select('lab_migration_experiment', 'e')
      ->fields('e')
      ->condition('id', $experiment_id)
      ->condition('proposal_id', $proposal_data->id)
      ->execute()
      ->fetchObject();

    if (!$experiment) {
      \Drupal::messenger()->addError($this->t('Invalid experiment.'));
      (new RedirectResponse(Url::fromRoute('lab_migration.list_experiments')->toString()))->send();
      return;
    }

    $code_number = $experiment->number . '.' . $form_state->getValue('code_number');

    $exists = \Drupal::database()->select('lab_migration_solution', 's')
      ->fields('s')
      ->condition('experiment_id', $experiment_id)
      ->condition('code_number', $code_number)
      ->execute()
      ->fetchObject();

    if ($exists) {
      \Drupal::messenger()->addError($this->t('Solution already exists.'));
      (new RedirectResponse(Url::fromRoute('lab_migration.list_experiments')->toString()))->send();
      return;
    }

    $solution_id = \Drupal::database()->insert('lab_migration_solution')->fields([
      'experiment_id' => $experiment_id,
      'approver_uid' => 0,
      'code_number' => $code_number,
      'caption' => $form_state->getValue('code_caption'),
      'approval_date' => 0,
      'approval_status' => 0,
      'timestamp' => time(),
      'os_used' => $form_state->getValue('os_used'),
      'esim_version' => $form_state->getValue('esim_version'),
      'toolbox_used' => 'none',
    ])->execute();

    $dest_path = "{$proposal_data->id}/EXP{$experiment->number}/CODE{$code_number}/";
    if (!is_dir($root_path . $dest_path)) {
      mkdir($root_path . $dest_path, 0777, TRUE);
    }

    foreach ($_FILES['files']['name'] as $file_key => $file_name) {
      if ($file_name && $_FILES['files']['tmp_name'][$file_key]) {
        $file_type = match (true) {
          str_contains($file_key, 'source') => 'S',
          str_contains($file_key, 'pdf') => 'P',
          default => 'U',
        };

        $target = $root_path . $dest_path . basename($file_name);
        if (move_uploaded_file($_FILES['files']['tmp_name'][$file_key], $target)) {
          \Drupal::database()->insert('lab_migration_solution_files')->fields([
            'solution_id' => $solution_id,
            'filename' => $file_name,
            'filepath' => $dest_path . $file_name,
            'pdfpath' => ($file_type == 'P') ? $dest_path . $file_name : '',
            'filemime' => $_FILES['files']['type'][$file_key],
            'filesize' => $_FILES['files']['size'][$file_key],
            'filetype' => $file_type,
            'timestamp' => time(),
          ])->execute();

          \Drupal::messenger()->addStatus($this->t('@name uploaded.', ['@name' => $file_name]));
        }
      }
    }

    \Drupal::messenger()->addStatus($this->t('Solution uploaded successfully.'));
    (new RedirectResponse(Url::fromRoute('lab_migration.list_experiments')->toString()))->send();
  }

  private function lab_migration_check_code_number($code_number) {
    return preg_match('/^[0-9]+$/', $code_number);
  }

  private function lab_migration_check_name($caption) {
    return preg_match('/^[a-zA-Z0-9 ]+$/', $caption);
  }

}
  
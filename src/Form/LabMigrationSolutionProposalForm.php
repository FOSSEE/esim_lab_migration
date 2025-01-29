<?php

/**
 * @file
 * Contains \Drupal\lab_migration\Form\LabMigrationSolutionProposalForm.
 */

namespace Drupal\lab_migration\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class LabMigrationSolutionProposalForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'lab_migration_solution_proposal_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $user = \Drupal::currentUser();

    $proposal_id = (int) arg(2);

    //$proposal_q = \Drupal::database()->query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
    $query = \Drupal::database()->select('lab_migration_proposal');
    $query->fields('lab_migration_proposal');
    $query->condition('id', $proposal_id);
    $proposal_q = $query->execute();
    $proposal_data = $proposal_q->fetchObject();
    if (!$proposal_data) {
      drupal_set_message("Invalid proposal.", 'error');
      drupal_goto('');
    }
    //var_dump($proposal_data->name); die;
    $form['name'] = [
      '#type' => 'item',
      '#markup' => l($proposal_data->name_title . ' ' . $proposal_data->name, 'user/' . $proposal_data->uid),
      '#title' => t('Proposer Name'),
    ];
    $form['lab_title'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->lab_title,
      '#title' => t('Title of the Lab'),
    ];

    $experiment_html = '';
    //$experiment_q = \Drupal::database()->query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d", $proposal_id);
    $query = \Drupal::database()->select('lab_migration_experiment');
    $query->fields('lab_migration_experiment');
    $query->condition('proposal_id', $proposal_id);
    $experiment_q = $query->execute();
    while ($experiment_data = $experiment_q->fetchObject()) {
      $experiment_html .= $experiment_data->title . "<br/>";
    }
    $form['experiment'] = [
      '#type' => 'item',
      '#markup' => $experiment_html,
      '#title' => t('Experiment List'),
    ];

    $form['solution_provider_name_title'] = [
      '#type' => 'select',
      '#title' => t('Title'),
      '#options' => [
        'Mr' => 'Mr',
        'Ms' => 'Ms',
        'Mrs' => 'Mrs',
        'Dr' => 'Dr',
        'Prof' => 'Prof',
      ],
      '#required' => TRUE,
    ];
    $form['solution_provider_name'] = [
      '#type' => 'textfield',
      '#title' => t('Name of the Solution Provider'),
      '#size' => 30,
      '#maxlength' => 50,
      '#required' => TRUE,
    ];
    $form['solution_provider_email_id'] = [
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#size' => 30,
      '#value' => $user->mail,
      '#disabled' => TRUE,
    ];
    $form['solution_provider_contact_ph'] = [
      '#type' => 'textfield',
      '#title' => t('Contact No.'),
      '#size' => 30,
      '#maxlength' => 15,
      '#required' => TRUE,
    ];
    $form['solution_provider_department'] = [
      '#type' => 'select',
      '#title' => t('Department/Branch'),
      '#options' => [
        '' => 'Please select...',
        'Computer Engineering' => 'Computer Engineering',
        'Electrical Engineering' => 'Electrical Engineering',
        'Electronics Engineering' => 'Electronics Engineering',
        'Chemical Engineering' => 'Chemical Engineering',
        'Instrumentation Engineering' => 'Instrumentation Engineering',
        'Mechanical Engineering' => 'Mechanical Engineering',
        'Civil Engineering' => 'Civil Engineering',
        'Physics' => 'Physics',
        'Mathematics' => 'Mathematics',
        'Others' => 'Others',
      ],
      '#required' => TRUE,
    ];
    $form['solution_provider_university'] = [
      '#type' => 'textfield',
      '#title' => t('University/Institute'),
      '#size' => 30,
      '#maxlength' => 50,
      '#required' => TRUE,
    ];

    $form['samplefile'] = [
      '#type' => 'fieldset',
      '#title' => t('Sample Source File'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    ];
    $form['samplefile']['samplefile1'] = [
      '#type' => 'file',
      '#title' => t('Upload sample source file'),
      '#size' => 48,
      '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions : ') . variable_get('textbook_companion_source_extensions', '') . '</span>',
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Apply for Solution'),
    ];
    return $form;
  }

  public function validateForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $user = \Drupal::currentUser();

    //$solution_provider_q = \Drupal::database()->query("SELECT * FROM {lab_migration_proposal} WHERE solution_provider_uid = ".$user->uid." AND approval_status IN (0, 1) AND solution_status IN (0, 1, 2)");
    $query = \Drupal::database()->select('lab_migration_proposal');
    $query->fields('lab_migration_proposal');
    $query->condition('solution_provider_uid', $user->uid);
    $query->condition('approval_status', [0, 1], 'IN');
    $query->condition('solution_status', [0, 1, 2], 'IN');
    $solution_provider_q = $query->execute();
    if ($solution_provider_q->fetchObject()) {
      $form_state->setErrorByName('', t("You have already applied for a solution. Please complete that before applying for another solution."));
      drupal_goto('lab_migration/open_proposal');
    }

    if (!($_FILES['files']['name']['samplefile1'])) {
      $form_state->setErrorByName('samplefile1', t('Please upload sample code main or source file.'));
    }

    if (isset($_FILES['files'])) {
      /* check if atleast one source or result file is uploaded */
      if (!($_FILES['files']['name']['samplefile1'])) {
        $form_state->setErrorByName('samplefile1', t('Please upload sample code main or source file.'));
      }

      /* check for valid filename extensions */
      foreach ($_FILES['files']['name'] as $file_form_name => $file_name) {
        if ($file_name) {
          /* checking file type */
          if (strstr($file_form_name, 'sample')) {
            $file_type = 'S';
          }
          else {
            $file_type = 'U';
          }

          $allowed_extensions_str = '';
          switch ($file_type) {
            case 'S':
              $allowed_extensions_str = variable_get('lab_migration_source_extensions', '');
              break;

          }
          $allowed_extensions = explode(',', $allowed_extensions_str);
          $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
          if (!in_array($temp_extension, $allowed_extensions)) {
            $form_state->setErrorByName($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
          }
          if ($_FILES['files']['size'][$file_form_name] <= 0) {
            $form_state->setErrorByName($file_form_name, t('File size cannot be zero.'));
          }

          /* check if valid file name */
          if (!lab_migration_check_valid_filename($_FILES['files']['name'][$file_form_name])) {
            $form_state->setErrorByName($file_form_name, t('Invalid file name specified. Only alphabets and numbers are allowed as a valid filename.'));
          }
        }
      }
    }

  }

  public function submitForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $user = \Drupal::currentUser();
    $root_path = lab_migration_samplecode_path();
    $proposal_id = (int) arg(2);

    //$proposal_q = \Drupal::database()->query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
    $query = \Drupal::database()->select('lab_migration_proposal');
    $query->fields('lab_migration_proposal');
    $query->condition('id', $proposal_id);
    $proposal_q = $query->execute();
    $proposal_data = $proposal_q->fetchObject();
    if (!$proposal_data) {
      drupal_set_message("Invalid proposal.", 'error');
      drupal_goto('lab_migration/open_proposal');
    }
    if ($proposal_data->solution_provider_uid != 0) {
      drupal_set_message("Someone has already applied for solving this Lab.", 'error');
      drupal_goto('lab_migration/open_proposal');
    }
    $actual_path = "";
    $dest_path = $proposal_id . '/';
    if (!is_dir($root_path . $dest_path)) {
      mkdir($root_path . $dest_path);
    }


    /* uploading files */
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name) {
      if ($file_name) {
        /* checking file type */
        $file_type = 'S';

        if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name])) {
          drupal_set_message(t("Error uploading file. File !filename already exists.", [
            '!filename' => $_FILES['files']['name'][$file_form_name]
            ]), 'error');
          drupal_goto('lab_migration/open_proposal');
          return;
        }

        /* uploading file */
        if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name])) {
          $actual_path = $dest_path . $_FILES['files']['name'][$file_form_name];

          drupal_set_message($file_name . ' uploaded successfully.', 'status');
        }
        else {
          drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
          drupal_goto('lab_migration/open_proposal');
        }
      }
    }


    $query = "UPDATE {lab_migration_proposal} set solution_provider_uid = :uid, solution_status = 1, solution_provider_name_title = :solution_provider_name_title, solution_provider_name = :solution_provider_contact_name, solution_provider_contact_ph = :solution_provider_contact_ph, solution_provider_department = :solution_provider_department, solution_provider_university = :solution_provider_university,samplefilepath=:samplefilepath WHERE id = :proposal_id";
    $args = [
      ":uid" => $user->uid,
      ":solution_provider_name_title" => $form_state->getValue(['solution_provider_name_title']),
      ":solution_provider_contact_name" => $form_state->getValue(['solution_provider_name']),
      ":solution_provider_contact_ph" => $form_state->getValue(['solution_provider_contact_ph']),
      ":solution_provider_department" => $form_state->getValue(['solution_provider_department']),
      ":solution_provider_university" => $form_state->getValue(['solution_provider_university']),
      ":samplefilepath" => $actual_path,
      ":proposal_id" => $proposal_id,
    ];
    $result = \Drupal::database()->query($query, $args);
    drupal_set_message("We have received your application. We will get back to you soon.", 'status');

    /* sending email */
    $email_to = $user->mail;

    $from = variable_get('lab_migration_from_email', '');
    $bcc = variable_get('lab_migration_emails', '');
    $cc = variable_get('lab_migration_cc_emails', '');

    $param['solution_proposal_received']['proposal_id'] = $proposal_id;
    $param['solution_proposal_received']['user_id'] = $user->uid;
    $param['solution_proposal_received']['headers'] = [
      'From' => $from,
      'MIME-Version' => '1.0',
      'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
      'Content-Transfer-Encoding' => '8Bit',
      'X-Mailer' => 'Drupal',
      'Cc' => $cc,
      'Bcc' => $bcc,
    ];

    if (!drupal_mail('lab_migration', 'solution_proposal_received', $email_to, language_default(), $param, $from, TRUE)) {
      drupal_set_message('Error sending email message.', 'error');
    }

    /* sending email */
    /* $email_to = variable_get('lab_migration_emails', '');
  if (!drupal_mail('lab_migration', 'solution_proposal_received', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');*/

    drupal_goto('lab_migration/open_proposal');
  }

}
?>

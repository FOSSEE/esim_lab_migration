<?php

/**
 * @file
 * Contains \Drupal\lab_migration\Form\LabMigrationProposalApprovalForm.
 */

namespace Drupal\lab_migration\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class LabMigrationProposalApprovalForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'lab_migration_proposal_approval_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $user = \Drupal::currentUser();

    /* get current proposal */
    $proposal_id = (int) arg(3);
    //$proposal_q = \Drupal::database()->query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
    $query = \Drupal::database()->select('lab_migration_proposal');
    $query->fields('lab_migration_proposal');
    $query->condition('id', $proposal_id);
    $proposal_q = $query->execute();

    if ($proposal_q) {
      if ($proposal_data = $proposal_q->fetchObject()) {
        /* everything ok */
      }
      else {
        drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
        drupal_goto('lab_migration/manage_proposal');
        return;
      }
    }
    else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
    // var_dump($proposal_data->name_title);
    //    die;
    $form['name'] = [
      '#type' => 'item',
      '#markup' => l($proposal_data->name_title . ' ' . $proposal_data->name, 'user/' . $proposal_data->uid),
      '#title' => t('Name'),
    ];
    $form['email_id'] = [
      '#type' => 'item',
      '#markup' => user_load($proposal_data->uid)->mail,
      '#title' => t('Email'),
    ];
    $form['contact_ph'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->contact_ph,
      '#title' => t('Contact No.'),
    ];
    $form['department'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->department,
      '#title' => t('Department/Branch'),
    ];
    $form['university'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->university,
      '#title' => t('University/Institute'),
    ];
    $form['country'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->country,
      '#title' => t('Country'),
    ];
    $form['all_state'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->state,
      '#title' => t('State'),
    ];
    $form['city'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->city,
      '#title' => t('City'),
    ];
    $form['pincode'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->pincode,
      '#title' => t('Pincode/Postal code'),
    ];
    $form['esim_version'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->esim_version,
      '#title' => t('eSim version used'),
    ];

    $form['lab_title'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->lab_title,
      '#title' => t('Title of the Lab'),
    ];

    /* get experiment details */
    $experiment_list = '<ul>';
    //$experiment_q = \Drupal::database()->query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY id ASC", $proposal_id);
    $query = \Drupal::database()->select('lab_migration_experiment');
    $query->fields('lab_migration_experiment');
    $query->condition('proposal_id', $proposal_id);
    $query->orderBy('id', 'ASC');
    $experiment_q = $query->execute();
    while ($experiment_data = $experiment_q->fetchObject()) {
      $experiment_list .= '<li>' . $experiment_data->title . '</li>Description of Experiment : ' . $experiment_data->description . '<br>';
    }
    $experiment_list .= '</ul>';

    $form['experiment'] = [
      '#type' => 'item',
      '#markup' => $experiment_list,
      '#title' => t('Experiments'),
    ];

    if ($proposal_data->solution_provider_uid == 0) {
      $solution_provider = "User will not provide solution, we will have to provide solution";
    }
    else {
      if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
        $solution_provider = "Proposer will provide the solution of the lab";
      }
      else {
        $solution_provider_user_data = user_load($proposal_data->solution_provider_uid);
        if ($solution_provider_user_data) {
          $solution_provider = "Solution will be provided by user " . l($solution_provider_user_data->name, 'user/' . $proposal_data->solution_provider_uid);
        }
        else {
          $solution_provider = "User does not exists";
        }
      }
    }
    $form['solution_provider_uid'] = [
      '#type' => 'item',
      '#title' => t('Do you want to provide the solution'),
      '#markup' => $solution_provider,
    ];
    if ($proposal_data->samplefilepath != "None") {
      if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
        $form['samplecode'] = [
          '#type' => 'markup',
          '#markup' => l('Download Sample Code', 'lab_migration/download/samplecode/' . $proposal_id) . "<br><br>" ,
        ];
      }
      else {
        if ($solution_provider_user_data) {
          $form['samplecode'] = [
            '#type' => 'markup',
            '#markup' => l('Download Sample Code', 'lab_migration/download/samplecode/' . $proposal_id) . "<br><br>" ,
          ];
        }
      }
    }
    $form['solution_display'] = [
      '#type' => 'hidden',
      '#title' => t('Do you want to display the solution on the www.esim.fossee.in website'),
      '#markup' => ($proposal_data->solution_display == 1) ? "Yes" : "No",
    ];

    $form['approval'] = [
      '#type' => 'radios',
      '#title' => t('Lab migration proposal'),
      '#options' => [
        '1' => 'Approve',
        '2' => 'Disapprove',
      ],
      '#required' => TRUE,
    ];

    $form['message'] = [
      '#type' => 'textarea',
      '#title' => t('Reason for disapproval'),
      '#states' => [
        'visible' => [
          ':input[name="approval"]' => [
            'value' => '2'
            ]
          ],
        'required' => [':input[name="approval"]' => ['value' => '2']],
      ],
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Submit'),
    ];

    $form['cancel'] = [
      '#type' => 'item',
      '#markup' => l(t('Cancel'), 'lab_migration/manage_proposal'),
    ];

    return $form;
  }

  public function validateForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    if ($form_state->getValue(['approval']) == 2) {
      if (strlen(trim($form_state->getValue(['message']))) <= 30) {
        $form_state->setErrorByName('approval', t('Please mention the reason for disapproval.'));
      }
    }
    return;

  }

  public function submitForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $user = \Drupal::currentUser();

    /* get current proposal */
    $proposal_id = (int) arg(3);
    // $proposal_q = \Drupal::database()->query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
    $query = \Drupal::database()->select('lab_migration_proposal');
    $query->fields('lab_migration_proposal');
    $query->condition('id', $proposal_id);
    $proposal_q = $query->execute();

    if ($proposal_q) {
      if ($proposal_data = $proposal_q->fetchObject()) {
        /* everything ok */
      }
      else {
        drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
        drupal_goto('lab_migration/manage_proposal');
        return;
      }
    }
    else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }

    if ($form_state->getValue(['approval']) == 1) {
      $query = "UPDATE {lab_migration_proposal} SET approver_uid = :uid, approval_date = :date, approval_status = 1, solution_status=2 WHERE id = :proposal_id";
      $args = [
        ":uid" => $user->uid,
        ":date" => time(),
        ":proposal_id" => $proposal_id,
      ];

      \Drupal::database()->query($query, $args);

      /* sending email */
      $user_data = user_load($proposal_data->uid);
      $email_to = $user_data->mail;

      $from = variable_get('lab_migration_from_email', '');
      $bcc = $user->mail . ', ' . variable_get('lab_migration_emails', '');
      $cc = variable_get('lab_migration_cc_emails', '');

      $param['proposal_approved']['proposal_id'] = $proposal_id;
      $param['proposal_approved']['user_id'] = $proposal_data->uid;
      $param['proposal_approved']['headers'] = [
        'From' => $from,
        'MIME-Version' => '1.0',
        'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
        'Content-Transfer-Encoding' => '8Bit',
        'X-Mailer' => 'Drupal',
        'Cc' => $cc,
        'Bcc' => $bcc,
      ];

      // echo "To: ".$email_to." From: ".variable_get('lab_migration_from_email', NULL)."<br>";
      // echo "<pre>";
      // var_dump($param); die;
    /*if (!drupal_mail('lab_migration', 'proposal_approved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))*/
      if (!drupal_mail('lab_migration', 'proposal_approved', $email_to, language_default(), $param, $from, TRUE)) {
        drupal_set_message('Error sending email message.', 'error');
      }

      /*$email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');
    if (!drupal_mail('lab_migration', 'proposal_approved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');*/

      drupal_set_message('Lab migration proposal No. ' . $proposal_id . ' approved. User has been notified of the approval.', 'status');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
    else {
      if ($form_state->getValue(['approval']) == 2) {
        $query = "UPDATE {lab_migration_proposal} SET approver_uid = :uid, approval_date = :date, approval_status = 2, message = :message, solution_provider_uid = 0, solution_status = 0 WHERE id = :proposal_id";
        $args = [
          ":uid" => $user->uid,
          ":date" => time(),
          ":message" => $form_state->getValue(['message']),
          ":proposal_id" => $proposal_id,
        ];
        $result = \Drupal::database()->query($query, $args);

        /* sending email */
        $user_data = user_load($proposal_data->uid);
        $email_to = $user_data->mail;

        $from = variable_get('lab_migration_from_email', '');
        $bcc = $user->mail . ', ' . variable_get('lab_migration_emails', '');
        $cc = variable_get('lab_migration_cc_emails', '');

        $param['proposal_disapproved']['proposal_id'] = $proposal_id;
        $param['proposal_disapproved']['user_id'] = $proposal_data->uid;
        $param['proposal_disapproved']['headers'] = [
          'From' => $from,
          'MIME-Version' => '1.0',
          'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
          'Content-Transfer-Encoding' => '8Bit',
          'X-Mailer' => 'Drupal',
          'Cc' => $cc,
          'Bcc' => $bcc,
        ];

        if (!drupal_mail('lab_migration', 'proposal_disapproved', $email_to, language_default(), $param, $from, TRUE)) {
          drupal_set_message('Error sending email message.', 'error');
        }

        /*$email_to = $user_data->mail . ', ' . variable_get('lab_migration_emails', '');;
    if (!drupal_mail('lab_migration', 'proposal_disapproved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');*/

        drupal_set_message('Lab migration proposal No. ' . $proposal_id . ' dis-approved. User has been notified of the dis-approval.', 'error');
        drupal_goto('lab_migration/manage_proposal');
        return;
      }
    }
  }

}
?>
